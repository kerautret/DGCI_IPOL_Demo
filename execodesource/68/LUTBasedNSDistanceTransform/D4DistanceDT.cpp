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
// $Id: D4DistanceDT.cpp 161 2014-03-27 16:32:50Z Nicolas.Normand $

#include <assert.h>
#include <limits.h>

#include <algorithm>

#include "D4DistanceDT.h"

D4Distance::D4Distance(GrayscalePixelType dMax) :
    _dMax(dMax) { }

BaseDistanceTransform* D4Distance::newTranslatedDistanceTransform(ImageConsumer<GrayscalePixelType>* consumer) const {
    return new D4DistanceTransform(consumer, _dMax);
}

DistanceTransformUntranslator<GrayscalePixelType, GrayscalePixelType>* D4Distance::newDistanceTransformUntranslator(ImageConsumer<GrayscalePixelType>* consumer) const {
    return new D4DistanceTransformUntranslator(consumer, _dMax);
}

void D4DistanceTransform::processRow(const BinaryPixelType *imageRow) {
    int col;
#define N1_COUNT  4
    static vect n1[N1_COUNT]   = {{-1, 1}, {0, 1}, {1, 1}, {0, 2}};

    for (col = 0; col < _cols; col++) {
	if (imageRow[col] == 0)
	    dtLines[0][col + 2] = 0;
	else {
	    GrayscalePixelType val;
	    int k;

	    // Ensure that the final output value is kept within the _dMax limit
	    val = _dMax - 1;
	    for (k = 0; k < N1_COUNT; k++) {
		assert(n1[k].y >= 0);
		assert(n1[k].y <= 2);
		assert(col + 2 - n1[k].x >= 0);
		assert(col + 2 - n1[k].x < _cols + 3);
		val = std::min(val, dtLines[n1[k].y][col + 2 - n1[k].x]);
	    }

	    dtLines[0][col + 2] = val + 1;
	}
    }
    _consumer->processRow(dtLines[0]+2);
    rotate();
}

D4DistanceTransform::D4DistanceTransform(ImageConsumer<GrayscalePixelType>* consumer, GrayscalePixelType dMax) :
    _dMax(dMax == 0 ? GRAYSCALE_MAX : dMax),
    BaseDistanceTransform(consumer) { }

D4DistanceTransformUntranslator::D4DistanceTransformUntranslator(ImageConsumer<GrayscalePixelType>* consumer, GrayscalePixelType dMax) :
    super(consumer, 0),
    _dMax(dMax == 0 ? GRAYSCALE_MAX : dMax) { }

D4DistanceTransformUntranslator::~D4DistanceTransformUntranslator() { }

void D4DistanceTransformUntranslator::beginOfImage(int cols, int rows) {
    int imageDMax = _dMax;
    imageDMax = std::min(imageDMax, (cols + 1) / 2);
    imageDMax = std::min(imageDMax, (rows + 1) / 2);
    super::beginOfImage(cols, rows, imageDMax + 2);
}

// Called once for each row of the input image, plus one extra time
// with null-valued translated DT to flush all DT values
void D4DistanceTransformUntranslator::processRow(const GrayscalePixelType* inputRow) {
    int dtmax = 1;  // Not 0 to avoid outputing the extra row

    super::processRow(inputRow);

    for (int col = 0; col < _cols; col++) {

	int dtn, dtp;
	int dy;

	dtp = _tdtRows[1][col];
	if (_tdtRows[0][col] == 0) {
	    assert(_outputRows[(_curRow + _dtRowCount) % _dtRowCount][col] == (GrayscalePixelType) -1);
	    _outputRows[(_curRow + _dtRowCount) % _dtRowCount][col] = 0;
	}

	dtn = _tdtRows[0][col];
	assert(dtn <= _dMax);
	dtmax = std::max(dtmax, dtn);
	dtn = std::max(0, dtn - 1);

	for (int r = dtn + 1;
	     r <= dtp;
	     r++) {

	    dy = r - 1;
	    assert(_curRow - 1 - dy >= 0);
	    assert(_outputRows[(_curRow - 1 - dy) % _dtRowCount][col] == (GrayscalePixelType) -1);
	    _outputRows[(_curRow - 1 - dy) % _dtRowCount][col] = r;
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
