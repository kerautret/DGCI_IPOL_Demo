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
// $Id: PGMImageWriter.cpp 150 2014-02-26 08:59:35Z Nicolas.Normand $

#include "PGMImageWriter.h"
#include <string.h>

PGMImageWriter::PGMImageWriter(FILE* output, int format) :
_cols(0),
_format(format),
_output(output) {
    _outpam.size        = sizeof(struct pam);
    _outpam.len         = sizeof(struct pam);
    _outpam.file        = output;
    _outpam.format      = PGM_FORMAT;
    _outpam.plainformat = 1;
}

void
PGMImageWriter::beginOfImage(int cols, int rows) {
    _cols = cols;
    _outpam.height	         = rows;
    _outpam.width	         = cols;
    _outpam.depth            = 1;
    _outpam.maxval           = 255;
    _outpam.bytes_per_sample = 1;
    strncpy(_outpam.tuple_type, PAM_PGM_TUPLETYPE, sizeof(_outpam.tuple_type));
    // EDIT BK 12/12 existe pas dans la version ipol (marche sans on dirait
    //_outpam.allocation_depth = sizeof(GrayscalePixelType);
#ifdef PAM_HAVE_COMMENT_P
    _outpam.comment_p        = NULL;
#endif
    _tuplerow = pnm_allocpamrow(&_outpam);
    pnm_writepaminit(&_outpam);
    //pgm_writepgminit(_output, cols, rows, 255, _format);
}

void
PGMImageWriter::endOfImage() {
    pnm_freepamrow(_tuplerow);
    _tuplerow = NULL;
}

void
PGMImageWriter::processRow(const GrayscalePixelType* inputRow) {
    // FIXME: type mismatch, change to pgm
    for (int column = 0; column < _outpam.width; ++column) {
	_tuplerow[column][0] = inputRow[column];
    }
    pnm_writepamrow(&_outpam, _tuplerow);
    //pgm_writepgmrow(_output, inputRow, _cols, 255, _format);
}
