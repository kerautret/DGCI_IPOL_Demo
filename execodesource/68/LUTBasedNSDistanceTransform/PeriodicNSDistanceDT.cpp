// Copyright 2012-2014 Nicolas Normand <Nicolas.Normand@polytech.univ-nantes.fr>
// 
// This file is part of LUTBasedNSDistanceTransform.
// 
// LUTBasedNSDistanceTransform is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
// 
// LUTBasedNSDistanceTransform is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with
// LUTBasedNSDistanceTransform.  If not, see <http://www.gnu.org/licenses/>.
//
// $Id: PeriodicNSDistanceDT.cpp 161 2014-03-27 16:32:50Z Nicolas.Normand $

#include <assert.h>
#include <limits.h>

#include <algorithm>

#include "PeriodicNSDistanceDT.h"
#include "CumulativeSequence.h"

int PeriodicNSDistance::countOf2(int r) const {
    return mathbf2d[r % period] + (((r + period - 1) / period) - 1) * mathbf2d[0];
}

inline GrayscalePixelType PeriodicNSDistance::next1(int r) const {
    return r + c1[r % period];
}

inline GrayscalePixelType PeriodicNSDistance::next2(int r) const {
    return r + c2[r % period];
}

PeriodicNSDistance::PeriodicNSDistance(int period, int *Bvalues, GrayscalePixelType dMax) :
    _dMax(dMax),
    period(period) {

    int i;
    for (i = 0; i < period; i++) {
	assert(Bvalues[i] == 1 || Bvalues[i] == 2);
	// FIXME: don't change Bvalues
	Bvalues[i] = 2 - Bvalues[i];
    }
    int *data = (int *)malloc(4 * period * sizeof(int));
    c1        = data;
    c2        = c1 + period;
    mathbf1d  = c2 + period;
    mathbf2d  = mathbf1d + period;

    CumulativeOfPeriodicSequence *mathbf1_B = CumulativeOfPeriodicSequenceCreate(period, 0, Bvalues);
    //CumulativeOfPeriodicSequencePrint(mathbf1_B);
    CumulativeOfPeriodicSequence *mathbf1_Binv = CumulativeOfPeriodicSequenceCreateInverse(mathbf1_B);
    //CumulativeOfPeriodicSequencePrint(mathbf1_Binv);

    for (i = 0; i < period; i++) {
	Bvalues[i] = 1 - Bvalues[i];
    }

    CumulativeOfPeriodicSequence *mathbf2_B = CumulativeOfPeriodicSequenceCreate(period, 0, Bvalues);
    CumulativeOfPeriodicSequence *mathbf2_Binv = CumulativeOfPeriodicSequenceCreateInverse(mathbf2_B);

    for (i = 0; i < period; i++) {
	Bvalues[i]++;
    }

    for (i = 0; i < period; i++) {
	c1[i] = CumulativeOfPeriodicSequenceValueAtIndex(mathbf1_Binv, CumulativeOfPeriodicSequenceValueAtIndex(mathbf1_B, i) + 1) + 1 - i;
	c2[i] = CumulativeOfPeriodicSequenceValueAtIndex(mathbf2_Binv, CumulativeOfPeriodicSequenceValueAtIndex(mathbf2_B, i) + 1) + 1 - i;
	mathbf1d[i] = CumulativeOfPeriodicSequenceValueAtIndex(mathbf1_B, i);
	mathbf2d[i] = CumulativeOfPeriodicSequenceValueAtIndex(mathbf2_B, i);
    }
    mathbf1d[0] = CumulativeOfPeriodicSequenceValueAtIndex(mathbf1_B, period);
    mathbf2d[0] = CumulativeOfPeriodicSequenceValueAtIndex(mathbf2_B, period);

    /*
    for (i = 1; i < 10; i++) {
	printf("%d %d, ", i, CumulativeOfPeriodicSequenceValueAtIndex(mathbf2_B, i));
    }
    printf("\n");
    for (i = 1; i < 10; i++) {
	printf("%d %d, ", (i - 1)/2, CumulativeOfPeriodicSequenceValueAtIndex(mathbf2_B, i - 1));
    }
    printf("\n");
    for (i = 1; i < 10; i++) {
	printf("%d %d, ", (i - 1)/2, i <= 2 ? 0 : data->mathbf2[((i - 2) % data->period)]
	       + ((i - 2) / data->period) * data->mathbf2[data->period - 1]);
    }
    printf("\n");*/

    CumulativeOfPeriodicSequenceFree(mathbf1_B);
    CumulativeOfPeriodicSequenceFree(mathbf1_Binv);
    CumulativeOfPeriodicSequenceFree(mathbf2_B);
    CumulativeOfPeriodicSequenceFree(mathbf2_Binv);
}

PeriodicNSDistance::~PeriodicNSDistance() {
    free(c1);
}

BaseDistanceTransform* PeriodicNSDistance::newTranslatedDistanceTransform(ImageConsumer<GrayscalePixelType>* consumer) const {
    return new PeriodicNSDistanceTransform(consumer, this, _dMax);
}

DistanceTransformUntranslator<GrayscalePixelType, GrayscalePixelType>* PeriodicNSDistance::newDistanceTransformUntranslator(ImageConsumer<GrayscalePixelType>* consumer) const {
    return new PeriodicNSDistanceTransformUntranslator(consumer, this, _dMax);
}

void PeriodicNSDistanceTransform::processRow(const BinaryPixelType *imageRow) {
    int col;
#define N1_SETMINUS_N2_COUNT  1
#define N2_SETMINUS_N1_COUNT  5
#define N1_CAP_N2_COUNT 3
    static vect n1[N1_SETMINUS_N2_COUNT]   = {{-1, 1}};
    static vect n2[N2_SETMINUS_N1_COUNT]   = {{1, 0}, {2, 0}, {2, 1}, {1, 2}, {2, 2}};
    static vect n12[N1_CAP_N2_COUNT] = {{0, 1}, {1, 1}, {0, 2}};

    for (col = 0; col < _cols; col++) {
	if (imageRow[col] == 0)
	    dtLines[0][col + 2] = 0;
	else {
	    GrayscalePixelType val;
	    GrayscalePixelType dt;
	    int k;

	    val = GRAYSCALE_MAX;
	    for (k = 0; k < N1_SETMINUS_N2_COUNT; k++) {
		assert(n1[k].y >= 0);
		assert(n1[k].y <= 2);
		assert(col + 2 - n1[k].x >= 0);
		assert(col + 2 - n1[k].x < _cols + 3);
		val = std::min(val, dtLines[n1[k].y][col + 2 - n1[k].x]);
	    }
	    dt = std::min(_dMax, _d->next1(val));

	    val = GRAYSCALE_MAX;
	    for (k = 0; k < N2_SETMINUS_N1_COUNT; k++) {
		assert(n2[k].y >= 0);
		assert(n2[k].y <= 2);
		assert(col + 2 - n2[k].x >= 0);
		assert(col + 2 - n2[k].x < _cols + 3);
		val = std::min(val, dtLines[n2[k].y][col + 2 - n2[k].x]);
	    }
	    dt = std::min(dt, _d->next2(val));

	    val = GRAYSCALE_MAX;
	    for (k = 0; k < N1_CAP_N2_COUNT; k++) {
		assert(n12[k].y >= 0);
		assert(n12[k].y <= 2);
		assert(col + 2 - n12[k].x >= 0);
		assert(col + 2 - n12[k].x < _cols + 3);
		val = std::min(val, dtLines[n12[k].y][col + 2 - n12[k].x]);
	    }
	    dt = std::min((int) dt, val + 1);

	    dtLines[0][col + 2] = dt;
	}
    }
    _consumer->processRow(dtLines[0]+2);
    rotate();
}

PeriodicNSDistanceTransform::PeriodicNSDistanceTransform(ImageConsumer<GrayscalePixelType>* consumer, const PeriodicNSDistance *d, GrayscalePixelType dMax) :
    BaseDistanceTransform(consumer),
    _dMax(dMax == 0 ? GRAYSCALE_MAX : dMax),
    _d(d) {
}

PeriodicNSDistanceTransform::~PeriodicNSDistanceTransform() {
}
							 
PeriodicNSDistanceTransformUntranslator::PeriodicNSDistanceTransformUntranslator(ImageConsumer<GrayscalePixelType>* consumer, const PeriodicNSDistance *d, GrayscalePixelType dMax) :
    super(consumer, marginRight),
    _dMax(dMax == 0 ? GRAYSCALE_MAX : dMax),
    _d(d) {
}

PeriodicNSDistanceTransformUntranslator::~PeriodicNSDistanceTransformUntranslator() {
}

void PeriodicNSDistanceTransformUntranslator::beginOfImage(int cols, int rows) {
    int imageDMax = _dMax;
    imageDMax = std::min(imageDMax, (cols + 1) / 2);
    imageDMax = std::min(imageDMax, (rows + 1) / 2);
    super::beginOfImage(cols, rows, imageDMax + 1);
}

// Called once for each row of the input image, plus one extra time
// with null-valued translated DT to flush all DT values
void PeriodicNSDistanceTransformUntranslator::processRow(const GrayscalePixelType* inputRow) {
    int dtmax = 1;  // Not 0 to avoid outputing the extra row

    super::processRow(inputRow);

    for (int col = 0; col < _cols; col++) {

	int dtn, dtp;

	dtp = _tdtRows[1][col];
	if (_tdtRows[0][col] == 0) {
	    assert(_outputRows[(_curRow + _dtRowCount) % _dtRowCount][col] == (GrayscalePixelType) -1);
	    _outputRows[(_curRow + _dtRowCount) % _dtRowCount][col] = 0;
	}

	dtn = _tdtRows[0][col];
	assert(dtn <= _dMax);
	dtmax = std::max(dtmax, dtn);
	dtn = std::max(0, dtn - 1);
	for (int r = _d->next1(dtn), dx = _d->countOf2(r - 1);
	     r <= dtp;
	     r = _d->next1(r)) {

	    int dy = r - 1;
	    assert(_outputRows[(_curRow - 1 - dy) % _dtRowCount][col - dx] == (GrayscalePixelType) -1);
	    _outputRows[(_curRow - 1 - dy) % _dtRowCount][col - dx] = r;
	    dx += _d->next1(r) - r - 1;
	}

	dtn = _tdtRows[0][col + 1];
	dtn = std::max(0, dtn - 1);
	for (int r = _d->next2(dtn), dx = _d->countOf2(r - 1);
	     r <= dtp;
	     r = _d->next2(r)) {

	    int dy = r - 1;
	    assert(_outputRows[(_curRow - 1 - dy) % _dtRowCount][col - dx] == (GrayscalePixelType) -1);
	    _outputRows[(_curRow - 1 - dy) % _dtRowCount][col - dx] = r;
	    dx++;
	}
    }
    _curRow++;
    for (; _outRow < _curRow - dtmax; _outRow++) {
	_consumer->processRow(_outputRows[_outRow % _dtRowCount]);
#ifndef NDEBUG
	for (int col = 0; col < _cols; col++) {
	    assert(_outputRows[_outRow % _dtRowCount][col] != (GrayscalePixelType) -1);
	    _outputRows[_outRow % _dtRowCount][col] = -1;
	}
#endif
    }
}
