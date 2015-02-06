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
// $Id: PBMImageReader.cpp 150 2014-02-26 08:59:35Z Nicolas.Normand $

/*
 *  PBMImageReader.cpp
 *  LUTBasedNSDistanceTransform
 */

#include "PBMImageReader.h"
extern "C" {
#include "pbm.h"
}

PBMImageReader::PBMImageReader(ImageConsumer<BinaryPixelType>* consumer, FILE *input) :
    super(consumer),
    _input(input)
{
}

void PBMImageReader::produceAllRows() {
    BinaryPixelType *inputRow;
    int cols, rows;
    int format;

    pbm_readpbminit(_input, &cols, &rows, &format);
    inputRow = pbm_allocrow(cols);
    _consumer->beginOfImage(cols, rows);
    for (int row = 0; row < rows; row++) {
	pbm_readpbmrow(_input, inputRow, cols, format);
	_consumer->processRow(inputRow);
    }
    _consumer->endOfImage();
    pbm_freerow(inputRow);
}
