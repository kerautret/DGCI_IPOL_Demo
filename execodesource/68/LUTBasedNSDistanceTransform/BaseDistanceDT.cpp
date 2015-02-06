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
// $Id: BaseDistanceDT.cpp 150 2014-02-26 08:59:35Z Nicolas.Normand $

#include <assert.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <algorithm>

#include "BaseDistanceDT.h"

void BaseDistanceTransform::rotate() {
    GrayscalePixelType *t = dtLines[2];
    dtLines[2] = dtLines[1];
    dtLines[1] = dtLines[0];
    dtLines[0] = t;
}

void BaseDistanceTransform::beginOfImage(int cols, int rows) {
    assert(!_inited);
    assert(_cols == 0);
    assert(dtLines[0] == NULL);
    assert(dtLines[1] == NULL);
    assert(dtLines[2] == NULL);

    _cols = cols;
    dtLines[0] = (GrayscalePixelType *) malloc((2 + cols + 1) * sizeof(GrayscalePixelType));
    memset(dtLines[0], 0, (2 + cols + 1) * sizeof(GrayscalePixelType));
    dtLines[1] = (GrayscalePixelType *) malloc((2 + cols + 1) * sizeof(GrayscalePixelType));
    memset(dtLines[1], 0, (2 + cols + 1) * sizeof(GrayscalePixelType));
    dtLines[2] =  (GrayscalePixelType *) malloc((2 + cols + 1) * sizeof(GrayscalePixelType));
    memset(dtLines[2], 0, (2 + cols + 1) * sizeof(GrayscalePixelType));

    _consumer->beginOfImage(cols, rows);

    _inited = true;
}

void BaseDistanceTransform::endOfImage() {
    _consumer->endOfImage();

    free(dtLines[0]);
    free(dtLines[1]);
    free(dtLines[2]);

    dtLines[0] = NULL;
    dtLines[1] = NULL;
    dtLines[2] = NULL;
    _cols = 0;
    _inited = false;
}

BaseDistanceTransform::BaseDistanceTransform(ImageConsumer<GrayscalePixelType>* consumer) :
super(consumer),
_inited(false),
_cols(0) {

    dtLines[0] = NULL;
    dtLines[1] = NULL;
    dtLines[2] = NULL;
}

BaseDistanceTransform::~BaseDistanceTransform() {
}
