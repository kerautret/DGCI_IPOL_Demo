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
// $Id: RationalBeattySequenceTest.cpp 150 2014-02-26 08:59:35Z Nicolas.Normand $

#include <assert.h>
#include <stdio.h>

#include "RationalBeattySequence.h"

void testSequence(int num, int den, int dir) {
    int i;

    RationalBeattySeq bs(num, den, dir);
    printf("Sequence: ");
    bs.print();
    for (i = 1; i < 15; i++) {
	printf("%d ", bs(i));
    }
    printf("\n\n");

    RationalBeattySeq bsc = bs.complement();
    printf("Complement of sequence: ");
    bsc.print();
    for (i = 1; i < 15; i++) {
	printf("%d ", bsc(i));
    }
    printf("\n\n");

    RationalBeattySeq bscc = bsc.complement();
    printf("Complement of complement of sequence: ");
    bscc.print();
    for (i = 1; i < 15; i++) {
	printf("%d ", bscc(i));
    }
    printf("\n\n");

    RationalBeattySeq bsi = bs.invert();
    printf("Inverse of sequence: ");
    bsi.print();
    for (i = 1; i < 15; i++) {
	printf("%d ", bsi(i));
    }
    printf("\n\n");

    RationalBeattySeq bsii = bsi.invert();
    printf("Inverse of inverse of sequence: ");
    bsii.print();
    for (i = 1; i < 15; i++) {
	printf("%d ", bsii(i));
    }
    printf("\n\n");

    assert(bs.equals(bsii));
    assert(bs.equals(bscc));
}

int main(int argc, char** argv) {
    testSequence(3, 2, 0);

    return 0;
}
