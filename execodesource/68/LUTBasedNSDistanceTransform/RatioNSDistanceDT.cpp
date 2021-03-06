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
// $Id: RatioNSDistanceDT.cpp 159 2014-03-25 18:21:50Z Nicolas.Normand $

#include <assert.h>
#include <limits.h>

#include <algorithm>

#include "RatioNSDistanceDT.h"

#ifndef NDEBUG
#define RBS(num, den, dir, n) (((n) * (num) - (dir) + (den)) / (den) - 1)
#define RBSi(num, den, dir, n) RBS(den, num, !(dir), n)
#define MATHBF2(num, den, n) RBS(num, den, 0, n)
#define MATHBF2i(num, den, n) RBSi(num, den, 0, n)
#define MATHBF1(num, den, n) (1 + RBS((den - num), den, 1, n))
#define MATHBF1i(num, den, n) RBSi((den - num), den, 1, (n - 1))
#define C1(num, den, n) (MATHBF1i(num, den, MATHBF1(num, den, n) + 1) + 1)
#define C2(num, den, n) (MATHBF2i(num, den, MATHBF2(num, den, n) + 1) + 1)
#endif

RatioNSDistance::RatioNSDistance(int num, int den, GrayscalePixelType dMax) :
_dMax(dMax),
num(num), den(den),
mbf1(RationalBeattySeq((den - num), den, den - 1)),
mbf2(RationalBeattySeq(num, den, 0)),
mbf1i(RationalBeattySeq((den - num), den, den - 1).invert()),
mbf2i(RationalBeattySeq(num, den, 0).invert()) {
#ifndef NDEBUG
    RationalBeattySeq B(num+den, den, 0);
    for (int n = 1; n < 1000; n++) {
	//printf("%d: %d %d %d %d %d\n", n, B(n)-B(n-1), data->mbf1(n), data->mbf2(n), data->mbf1i(n), data->mbf2i(n));
	assert(mbf1(n) + mbf2(n) == n);
	assert(mbf1(n) - mbf1(n - 1) == (B(n)-B(n-1) == 1));
	assert(mbf2(n) - mbf2(n - 1) == (B(n)-B(n-1) == 2));
	assert(MATHBF1(num, den, n) == mbf1(n));
	assert(MATHBF2(num, den, n) == mbf2(n));
	assert(MATHBF1i(num, den, n) == mbf1i(n));
	assert(MATHBF2i(num, den, n) == mbf2i(n));
    }
#endif
}

BaseDistanceTransform* RatioNSDistance::newTranslatedDistanceTransform(ImageConsumer<GrayscalePixelType>* consumer) const {
    return new RatioNSDistanceTransform(consumer, num, den, _dMax);
}

DistanceTransformUntranslator<GrayscalePixelType, GrayscalePixelType>* RatioNSDistance::newDistanceTransformUntranslator(ImageConsumer<GrayscalePixelType>* consumer) const {
    return new RatioNSDistanceTransformUntranslator(consumer, num, den, _dMax);
}

void RatioNSDistanceTransform::processRow(const BinaryPixelType *imageRow) {
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
	    assert(C1(d.num, d.den, (int) val) == d.mbf1i(d.mbf1(val)+1)+1);
	    dt = std::min((int)_dMax, d.mbf1i(d.mbf1(val)+1)+1);

	    val = GRAYSCALE_MAX;
	    for (k = 0; k < N2_SETMINUS_N1_COUNT; k++) {
		assert(n2[k].y >= 0);
		assert(n2[k].y <= 2);
		assert(col + 2 - n2[k].x >= 0);
		assert(col + 2 - n2[k].x < _cols + 3);
		val = std::min(val, dtLines[n2[k].y][col + 2 - n2[k].x]);
	    }
	    assert(C2(d.num, d.den, (int) val) == d.mbf2i(d.mbf2(val)+1)+1);
	    dt = std::min((int) dt, d.mbf2i(d.mbf2(val)+1)+1);

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

RatioNSDistanceTransform::RatioNSDistanceTransform(ImageConsumer<GrayscalePixelType>* consumer, int num, int den, GrayscalePixelType dMax) :
    BaseDistanceTransform(consumer),
    _dMax(dMax == 0 ? GRAYSCALE_MAX : dMax),    
    d(num, den) {
}

RatioNSDistanceTransformUntranslator::RatioNSDistanceTransformUntranslator(ImageConsumer<GrayscalePixelType>* consumer, int num, int den, GrayscalePixelType dMax) :
super(consumer, marginRight),
    _dMax(dMax == 0 ? GRAYSCALE_MAX : dMax),
    d(num, den) {
}

RatioNSDistanceTransformUntranslator::~RatioNSDistanceTransformUntranslator() {
}

void RatioNSDistanceTransformUntranslator::beginOfImage(int cols, int rows) {
    int imageDMax = _dMax;
    imageDMax = std::min(imageDMax, (cols + 1) / 2);
    imageDMax = std::min(imageDMax, (rows + 1) / 2);
    super::beginOfImage(cols, rows, imageDMax + 1);
}

// untranslate is called once for each row of the input image, plus one extra time
// with null-valued translated DT to flush all DT values
void RatioNSDistanceTransformUntranslator::processRow(const GrayscalePixelType* inputRow) {
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
	//printf("row %d col %d: [%d, %d] ", _curRow, col - 2, _tdtRows[0][col], dtp);
	assert(C1(d.num, d.den, dtn) == d.mbf1i(d.mbf1(dtn)+1)+1);
	//printf("%d vs. %d\n", MATHBF2(d.num, den, r - 1), d.mbf2(r-1));
	assert(MATHBF2(d.num, d.den, d.mbf1i(d.mbf1(dtn)+1)) == d.mbf2(d.mbf1i(d.mbf1(dtn)+1)));
	for (int r = d.mbf1i(d.mbf1(dtn)+1)+1, dx = d.mbf2(r-1);
	     r <= dtp;
	     r = d.mbf1i(d.mbf1(r)+1)+1) {

	    int dy = r - 1;
	    assert (dx == d.mbf2(r-1));
	    assert(_curRow - 1 - dy >= 0);
	    assert(_outputRows[(_curRow - 1 - dy) % _dtRowCount][col - dx] == (GrayscalePixelType) -1);
	    _outputRows[(_curRow - 1 - dy) % _dtRowCount][col - dx] = r;
	    //printf("%d ", r);
	    // Let s be the next radius where neighborhood 1 is used
	    // s = r + c1[r % period]
	    // between r and s, neighborhood 2 is used s - r - 1 times
	    // so dx is raised by s - r - 1 = c1[r % period] - 1
	    //assert(dx + c1[r % period] - 1 == mathbf2(d, r + c1[r % period] - 1));
	    //assert(dx + C1(d.num, den, r) - r - 1 == MATHBF2(d.num, den, C1(d.num, den, r) - 1));
	    //dx += C1(d.num, den, r) - r - 1;
	    assert(MATHBF2(d.num, d.den, C1(d.num, d.den, r) - 1) == d.mbf2(d.mbf1i(d.mbf1(r)+1)));
	    dx = d.mbf2(d.mbf1i(d.mbf1(r)+1));
	    assert(C1(d.num, d.den, r) == d.mbf1i(d.mbf1(r)+1)+1);
	}
	//printf("\n");

	dtn = _tdtRows[0][col + 1];
	dtn = std::max(0, dtn - 1);
	//printf("row %d col %d: [%d, %d] ", _curRow, col - 2, _tdtRows[0][col + 1], dtp);
	assert(C2(d.num, d.den, dtn) == d.mbf2i(d.mbf2(dtn)+1)+1);
	//printf("%d vs. %d\n", MATHBF2(d.num, den, r - 1), d.mbf2(r-1));
	assert(MATHBF2(d.num, d.den, d.mbf2i(d.mbf2(dtn)+1)) == d.mbf2(d.mbf2i(d.mbf2(dtn)+1)));
	for (int r = d.mbf2i(d.mbf2(dtn)+1)+1, dx = d.mbf2(d.mbf2i(d.mbf2(dtn)+1));
	     r <= dtp;
	     r = d.mbf2i(d.mbf2(r)+1)+1) {

	    int dy = r - 1;
	    assert(dx == MATHBF2(d.num, d.den, r - 1));

	    assert(_curRow - 1 - dy >= 0);
	    assert(_outputRows[(_curRow - 1 - dy) % _dtRowCount][col - dx] == (GrayscalePixelType) -1);
	    _outputRows[(_curRow - 1 - dy) % _dtRowCount][col - dx] = r;
	    //printf("%d ", r);
	    // Next time we use neighborhood 2, dx is increased by one
	    assert(dx + 1 == MATHBF2(d.num, d.den, C2(d.num, d.den, r) - 1));
	    dx++;
	    assert(C2(d.num, d.den, r) == d.mbf2i(d.mbf2(r)+1)+1);
	}
	//printf("\n");
    }
    _curRow++;
    //printf("max: %d\n", dtmax);
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
