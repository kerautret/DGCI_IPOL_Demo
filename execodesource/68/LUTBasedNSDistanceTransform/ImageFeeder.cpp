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
// $Id: ImageFeeder.cpp 153 2014-02-26 10:50:16Z Nicolas.Normand $

extern "C" {
#include <pbm.h>
#include <pgm.h>
}
#include <assert.h>
#include <limits.h>

int main(int argc, char** argv) {
    int row;
    int cols;
    int rows;
    int format;
    bit *imageRow;

    pm_init(argv[0], 0);

    pbm_readpbminit(stdin, &cols, &rows, &format);
    pbm_writepbminit(stdout, cols, rows, 1);
    imageRow = pbm_allocrow(cols);

    for (row = 0; row < rows; row++) {
	if (row > 0) usleep(500000);
	pbm_readpbmrow(stdin, imageRow, cols, format);
	pbm_writepbmrow(stdout, imageRow, cols, 1);
	fflush(stdout);
    }

    pgm_freerow(imageRow);

    return 0;
}
