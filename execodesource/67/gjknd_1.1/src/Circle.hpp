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

#ifndef DLL_CIRCLE_HPP
#define DLL_CIRCLE_HPP

#include <iostream>
#include "GJK_nD.hpp"

namespace DLL {

/**
  \class Circle
  \brief A DLL model to represent digital circles.

  The Circle class reprensents DLL that have an analytical representation of
  the form: \htmlonly R<sub>min</sub><sup>2</sup> &le;
  (x - c<sub>x</sub>)<sup>2</sup> + (y - c<sub>y</sub>)<sup>2</sup> &le;
  R<sub>max</sub><sup>2</sup>\endhtmlonly.
 */
class Circle
{
private:
  typedef std::pair<size_t, size_t>   Coordinates;
  typedef GJKnD::Vector               Vector;
  typedef GJKnD::Point                Point;

public:
  /**
    Add the point \a xy to the set that has to lie inside the DLL.
   */
  void addInPoint  (const Coordinates & xy) { addPointToSet(xy, setIn);   }

  /**
    Add the point \a xy to the set that has to lie on the negative side
    outside of the DLL.
   */
  void addDownPoint(const Coordinates & xy) { addPointToSet(xy, setDown); }

  /**
    Add the point \a xy to the set that has to lie on the positive side
    outside of the DLL.
   */
  void addUpPoint  (const Coordinates & xy) { addPointToSet(xy, setUp);   }

  /**
    Do the recognition process with the current sets of points and returns true
    if the DLL is still growable after we've added points. If it returns false,
    the DLL is not growable anymore and its reliable characteristics are the
    ones we got from the last call to stillGrowableAfterUpdate() that returned
    true.
    */
  bool stillGrowableAfterUpdate();

  /**
    Print the characteristics of the \a circle on the \a out stream.
   */
  friend std::ostream & operator<<(std::ostream & out, const Circle & circle);

private:
  void addPointToSet(const Coordinates & xy, std::vector<Point> & set);

private:
  bool myIsDegenerated;

  std::vector<Point> setIn;
  std::vector<Point> setDown;
  std::vector<Point> setUp;

  double cx, cy, radius_above, radius_below;
};

} // namespace DLL

#endif // DLL_CIRCLE_HPP
