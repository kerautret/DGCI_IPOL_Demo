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
// $Id: CumulativeSequenceTest.cpp 150 2014-02-26 08:59:35Z Nicolas.Normand $

#include <assert.h>
#include <stdio.h>

#include "CumulativeSequence.h"

void testSequence(int period, int offset, int *sequence) {
    int i;
    CumulativeOfPeriodicSequence *cs, *csi, *csii;

    printf("Sequence: ");
    cs = CumulativeOfPeriodicSequenceCreate(period, offset, sequence);
    for (i = 1; i < 15; i++) {
	printf("%d ", CumulativeOfPeriodicSequenceValueAtIndex(cs, i));
    }
    CumulativeOfPeriodicSequencePrint(cs);

    printf("Inverse of sequence: ");
    csi = CumulativeOfPeriodicSequenceCreateInverse(cs);
    for (i = 1; i < 15; i++) {
	printf("%d ", CumulativeOfPeriodicSequenceValueAtIndex(csi, i));
    }
    CumulativeOfPeriodicSequencePrint(csi);

    printf("Inverse of inverse of sequence: ");
    csii = CumulativeOfPeriodicSequenceCreateInverse(csi);
    for (i = 1; i < 15; i++) {
	printf("%d ", CumulativeOfPeriodicSequenceValueAtIndex(csii, i));
    }
    CumulativeOfPeriodicSequencePrint(csii);

    printf("\n");

    assert(CumulativeOfPeriodicSequenceEquals(cs, csii));

    CumulativeOfPeriodicSequenceFree(cs);
    CumulativeOfPeriodicSequenceFree(csi);
    CumulativeOfPeriodicSequenceFree(csii);
}

int main(int argc, char** argv) {
    int period;
    int seq2[4] = {2, 0, 0, 1};
    period = sizeof(seq2) / sizeof(seq2[0]);

    testSequence(period,-4, seq2);

    testSequence(period,-1, seq2);
    testSequence(period, 0, seq2);
    testSequence(period, 1, seq2);
    testSequence(period, 10, seq2);
    testSequence(period, 100, seq2);
    testSequence(period,-10, seq2);
    testSequence(period,-100, seq2);

    int sequence[4] = {1, 2, 0, 3};	// cf. Table 1
    period = sizeof(sequence) / sizeof(sequence[0]);

    testSequence(period, 0, sequence);

    return 0;
}
