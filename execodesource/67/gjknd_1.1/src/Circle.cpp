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

#include "Circle.hpp"
#include <cmath>

namespace DLL {

/**
  This function adds the point \a xy to the specified \a set by using a
  transformation (Kernel Trick) to turn the 2D recognition problem into
  a 3D recognition problem, where constraints are linears.
 */
void Circle::addPointToSet(const Coordinates & xy, std::vector<Point> & set)
{
  // Kernel-trick: we project the points on the paraboloid z = x^2 + y^2
  Point augmentedPoint(3);

  double x = xy.first;
  double y = xy.second;

  augmentedPoint(0) = x;
  augmentedPoint(1) = y;
  augmentedPoint(2) = x * x + y * y;

  set.push_back(augmentedPoint);
}


bool Circle::stillGrowableAfterUpdate()
{
  Vector normal(3);
  double h, H;

  bool isCircle;
  try {
    isCircle = GJKnD::isDigitalHyperplane(setIn, setUp, setDown,
                                          normal, h, H);
  }
  catch (const SingularSystem &) {
    isCircle = false;
  }
  catch (const SuspiciousLoop &) {
    isCircle = false;
  }

  if (isCircle) {
    double coeff = normal(2);
    if (std::fabs(coeff) < 1.0e-12) {
      // degenerate case
      // the equation describes a StraightLine DLL
      cx = normal(0);
      cy = normal(1);
      radius_above = H;
      radius_above = h;
      myIsDegenerated = true;
    }
    else
    {
      // radius is finite
      // compute the center and radius
      cx = normal(0);
      cy = normal(1);

      cx = -cx / (2 * coeff);
      cy = -cy / (2 * coeff);

      double cx2cy2 = cx * cx + cy * cy;
      radius_above = std::sqrt(H / coeff + cx2cy2);
      radius_below = std::sqrt(h / coeff + cx2cy2);
      myIsDegenerated = false;
    }
  }

  return isCircle;
}


std::ostream & operator<<(std::ostream & out, const Circle & circle)
{
  if (circle.myIsDegenerated) {
    out << circle.radius_below << " <= "
        << circle.cx << "x " << (circle.cy > 0.0 ? "+" : "") << circle.cy
        << "y <= " << circle.radius_above;
  }
  else {
    out << circle.radius_below << " <= "
        << "(x" << (circle.cx > 0.0 ? "+" : "") << circle.cx << ")^2 + "
        << "(y" << (circle.cy > 0.0 ? "+" : "") << circle.cy << ")^2"
        << " <= " << circle.radius_above;
  }

  return out;
}

} // namespace DLL
