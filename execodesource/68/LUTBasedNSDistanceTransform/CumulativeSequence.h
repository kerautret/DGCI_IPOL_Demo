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
// $Id: CumulativeSequence.h 150 2014-02-26 08:59:35Z Nicolas.Normand $

extern "C" {
typedef struct CumulativeOfPeriodicSequence CumulativeOfPeriodicSequence;

CumulativeOfPeriodicSequence *CumulativeOfPeriodicSequenceCreate(int period, int offset, int *values);

CumulativeOfPeriodicSequence *CumulativeOfPeriodicSequenceCreateInverse(CumulativeOfPeriodicSequence *seq);

void CumulativeOfPeriodicSequenceFree(CumulativeOfPeriodicSequence *seq);

int CumulativeOfPeriodicSequenceValueAtIndex(CumulativeOfPeriodicSequence *seq, int i);

int CumulativeOfPeriodicSequenceEquals(CumulativeOfPeriodicSequence *seq1, CumulativeOfPeriodicSequence *seq2);

void CumulativeOfPeriodicSequencePrint(CumulativeOfPeriodicSequence *seq);
}