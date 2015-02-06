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
// $Id: Table1.cpp 150 2014-02-26 08:59:35Z Nicolas.Normand $

#include <assert.h>
#include <stdio.h>

#include "CumulativeSequence.h"

void testSequence(int period, int offset, int *sequence) {
    int n;
    CumulativeOfPeriodicSequence *cs, *csi;

    printf("f(n): ");
    cs = CumulativeOfPeriodicSequenceCreate(period, offset, sequence);
    for (n = 1; n < 15; n++) {
	printf("%d ", CumulativeOfPeriodicSequenceValueAtIndex(cs, n));
    }
    CumulativeOfPeriodicSequencePrint(cs);

    printf("f†(n): ");
    csi = CumulativeOfPeriodicSequenceCreateInverse(cs);
    for (n = 1; n < 15; n++) {
	printf("%d ", CumulativeOfPeriodicSequenceValueAtIndex(csi, n));
    }
    CumulativeOfPeriodicSequencePrint(csi);

    printf("f†(f(n)+1)+1: ");
    for (n = 1; n < 15; n++) {
	printf("%d ", CumulativeOfPeriodicSequenceValueAtIndex(
		          csi,
			  CumulativeOfPeriodicSequenceValueAtIndex(cs, n) + 1) + 1);
    }

    printf("\n");

    CumulativeOfPeriodicSequenceFree(cs);
    CumulativeOfPeriodicSequenceFree(csi);
}

int main(int argc, char** argv) {
    int period;
    int sequence[4] = {1, 2, 0, 3};	// cf. Table 1
    period = sizeof(sequence) / sizeof(sequence[0]);

    testSequence(period, 0, sequence);

    return 0;
}
