/*
 * Copyright (c) 2012   Laurent Provot <provot.research@gmail.com>,
 * Yan Gerard <yan.gerard@free.fr> and Fabien Feschet <research@feschet.fr>
 * All rights reserved.
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <fstream>
#include <iostream>
#include "GJK_nD.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  typedef GJKnD::Point    Point;
  typedef GJKnD::Vector   Vector;
  vector<Point> setIn, setAbove, setBelow;

  // argv[1] = setIn file, argv[2] = setAbove file and argv[3] = setBelow file
  assert(argc == 4);

  size_t dim;

  // read setIn
  ifstream fileIn(argv[1]);
  if (!fileIn.is_open()) {
    cerr << "Unable to open " << argv[1] << " to read setIn !" << endl;
    exit(-1);
  }
  fileIn >> dim;
  Point point(dim);
  while (!fileIn.eof()) {
    for (size_t i = 0; i < dim; ++i)
      fileIn >> point(i);
    fileIn >> ws;
    setIn.push_back(point);
  }
  fileIn.close();

  // read setAbove
  ifstream fileAbove(argv[2]);
  if (!fileAbove.is_open()) {
    cerr << "Unable to open " << argv[2] << " to read setAbove !" << endl;
    exit(-1);
  }
  fileAbove >> dim;
  while (!fileAbove.eof()) {
    for (size_t i = 0; i < dim; ++i)
      fileAbove >> point(i);
    fileAbove >> ws;
    setAbove.push_back(point);
  }
  fileAbove.close();

  // read setBelow
  ifstream fileBelow(argv[3]);
  if (!fileBelow.is_open()) {
    cerr << "Unable to open " << argv[3] << " to read setBelow !" << endl;
    exit(-1);
  }
  fileBelow >> dim;
  while (!fileBelow.eof()) {
    for (size_t i = 0; i < dim; ++i)
      fileBelow >> point(i);
    fileBelow >> ws;
    setBelow.push_back(point);
  }
  fileBelow.close();

  double h, H;
  Vector normal;
  if (GJKnD::isDigitalHyperplane(setIn, setAbove, setBelow, normal, h, H)) {
    cout << "A couple of separable parallel hyperplanes has been found.\n"
         << "Normal vector: " << normal
         << "Lower bound: " << h << " and upper bound: " << H
         << endl;
  }
  else {
    cout << "There is no couple of separable parallel hyperplanes !" << endl;
  }

  return 0;
}
