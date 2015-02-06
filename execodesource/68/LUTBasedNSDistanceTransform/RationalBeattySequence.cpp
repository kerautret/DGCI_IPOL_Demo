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
// $Id: RationalBeattySequence.cpp 150 2014-02-26 08:59:35Z Nicolas.Normand $

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "RationalBeattySequence.h"

struct RationalBeattySequence {
    int num;
    int den;
    int dir;	// Rounding direction: 0 for floor, 1 for ceil
};

RationalBeattySequence *RationalBeattySequenceCreate(int num, int den, int dir) {
    assert(num > 0);
    assert(den > 0);
    RationalBeattySequence *seq = (RationalBeattySequence *)malloc(sizeof(RationalBeattySequence));
    if (seq != NULL) {
	seq->num = num;
	seq->den = den;
	seq->dir = dir;
    }
    return seq;
}

RationalBeattySequence *RationalBeattySequenceCreateInverse(RationalBeattySequence *seq) {
    return RationalBeattySequenceCreate(seq->den, seq->num, !seq->dir);
}

RationalBeattySequence *RationalBeattySequenceCreateComplement(RationalBeattySequence *seq) {
    // 1 + 1/(tau - 1) -> tau/(tau - 1) -> num/(den * (num/den - 1)) ->
    // num/(num - den)
    return RationalBeattySequenceCreate(seq->num, seq->num - seq->den, !seq->dir);
}

void RationalBeattySequenceFree(RationalBeattySequence *seq) {
    free(seq);
}

int RationalBeattySequenceValueAtIndex(RationalBeattySequence *seq, int n) {
    assert(n >= 0);
    // Floor dir: floor(n*tau) -> (n*num)/den
    // Ceil dir: ceil(n*tau - 1) -> (n*num+den-1)/den - 1 -> (n*num-1)/den
    return (n * seq->num - seq->dir) / seq->den;
}

void RationalBeattySequencePrint(RationalBeattySequence *seq) {
    printf("num: %d, den: %d, dir: %d\n", seq->num, seq->den, seq->dir);
}

int RationalBeattySequenceEquals(RationalBeattySequence *seq1, RationalBeattySequence *seq2) {
    return memcmp(seq1, seq2, sizeof(RationalBeattySequence)) == 0;
}

int mod(int a, int b) {
    return ((a % b) + b) % b;
}
