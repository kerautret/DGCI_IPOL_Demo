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

#include "StraightLine.hpp"
#include <cmath>

namespace DLL {

/**
  This function adds the point \a xy to the specified \a set.
 */
void StraightLine::addPointToSet(const Coordinates & xy,
                                 std::vector<Point> & set)
{
  // Kernel-trick: we use GJK_nD as is, because a straight line is
  //               an hyperplane in 2D
  Point point(2);

  point(0) = xy.first;
  point(1) = xy.second;

  set.push_back(point);
}


bool StraightLine::stillGrowableAfterUpdate()
{
  Vector normal(2);
  double h, H;

  bool isStraightLine;
  try {
    isStraightLine = GJKnD::isDigitalHyperplane(setIn, setUp, setDown,
                                                normal, h, H);
  }
  catch (const SingularSystem &) {
    isStraightLine = false;
  }
  catch (const SuspiciousLoop &) {
    isStraightLine = false;
  }

  if (isStraightLine) {
    a = normal(0);
    b = normal(1);
    upper_bound = H;
    lower_bound = h;
  }

  return isStraightLine;
}


std::ostream & operator<<(std::ostream & out, const StraightLine & line)
{
  out << line.lower_bound << " <= "
      << line.a << "x " << (line.b > 0 ? "+" : "") << line.b
      << "y <= " << line.upper_bound;

  return out;
}

} // namespace DLL
