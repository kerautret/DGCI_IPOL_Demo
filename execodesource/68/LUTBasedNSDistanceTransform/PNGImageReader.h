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
// $Id: PNGImageReader.h 150 2014-02-26 08:59:35Z Nicolas.Normand $

/*
 *  PNGImageReader.h
 *  LUTBasedNSDistanceTransform
 */

#include "ImageFilter.h"
#include "png.h"

class PNGImageReader: public RowImageProducer<BinaryPixelType> {
public:
    PNGImageReader(ImageConsumer<BinaryPixelType>* consumer, FILE *input);

    void produceAllRows(size_t readBytes);

private:
    void startImage(size_t readBytes = 0);

    png_structp _png_ptr;
    png_infop _info_ptr;

    png_uint_32 _cols, _rows;
    FILE *_input;
    BinaryPixelType *_inputRow;
};
