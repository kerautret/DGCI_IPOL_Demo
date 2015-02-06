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

#ifndef DLL_SEGMENT_HPP
#define DLL_SEGMENT_HPP

#include <vector>
#include <utility>
#include <iostream>
#include "GJK_nD.hpp"

namespace DLL {

/**
  \class Segment
  \brief Represents a potentially growable piece of DLL (wrt. DLL_Model).

  The Segment class represents a piece of a 8-connected DLL that can
  incrementaly grow by addition of points. At the same time, 4-connected
  neigbhors of the segment's points are marked as forbidden points and must
  stay out of the DLL.

  \tparam DLL_Model the underlying DLL type of the segment. It must provide:

  - a <b>bool stillGrowableAfterUpdate()</b> function that returns true if the
    DLL is still growable after we've added points. If it returns false, the
    DLL is not growable anymore and its reliable characteristics are the
    ones we got from the last call to stillGrowableAfterUpdate() that returned
    true,
  - a <b>addInPoint(Coordinates)</b> to add points to the set that has to lie
    inside the DLL,
  - a <b>addUpPoint(Coordinates)</b> to add points to the set that has to lie
    on the positive side outside of the DLL,
  - a <b>addDownPoint(Coordinates)</b> to add points to the set that has to lie
    on the negative side outside of the DLL.

  \code
  \endcode
 */
template <typename DLL_Model>
class Segment
{
public:
  /// The coordinates type of each pixel of the curve
  typedef std::pair<size_t, size_t>   Coordinates;
  /// The type of a curve
  typedef std::vector<Coordinates>    Curve;

public:
  /**
    Construct an empty Segment, not yet valid, but growable.
   */
  Segment() : myIsValid(false), myCanGrow(true) {}

  /**
    Add the point \a xy to the current segment and implicitely update the set
    of forbidden points of the underlying DLL.
    \return true if we can continue to add points to the segment and false
    otherwise
   */
  bool addPoint(const Coordinates & xy);

  /**
    Return true if the segments (i.e. this piece of DLL) is valid and false
    otherwise (i.e. the DLL does not contains more than 2 points).
   */
  bool isValid() const { return myIsValid; }

  /**
    Return the set of points that compose the segment.
   */
  const Curve & getCurve() const { return myCurve; }

  /**
    Print the characteristics of the \a dll segment on the \a out stream.
   */
  template <typename U>
  friend std::ostream & operator<<(std::ostream & out, const Segment<U> & dll);

  /// \cond TEST
  // Used for test purpose only
  const Curve & getUpCurve() const { return myUpNeighbors; }
  /// \endcond

  /// \cond TEST
  // Used for test purpose only
  const Curve & getDownCurve() const { return myDownNeighbors; }
  /// \endcond

private:
  template <int N>
  void updateConnectedUpDownCurves();
  unsigned int computeFreemanCode(const Coordinates & c1,
                                  const Coordinates & c2) const;

private:
  static const int xFromFreeman[];
  static const int yFromFreeman[];

private:
  bool myIsValid;
  bool myCanGrow;

  Curve myCurve;
  Curve myUpNeighbors;
  Curve myDownNeighbors;

  DLL_Model myUnderliyingModel;
};


template <typename DLL_Model>
const int
Segment<DLL_Model>::xFromFreeman[] = {1, 1, 0, -1, -1, -1,  0,  1};

template <typename DLL_Model>
const int
Segment<DLL_Model>::yFromFreeman[] = {0, 1 ,1,  1,  0, -1, -1, -1};


template <typename DLL_Model>
bool Segment<DLL_Model>::addPoint(const Coordinates & xy)
{
  // we need at least two points in the curve in order to start
  // the recognition process at the next iteration
  if (myCurve.size() < 2) {
    myCurve.push_back(xy);
    myUnderliyingModel.addInPoint(xy);
    if (myCurve.size() == 2)
      myIsValid = true;
    return true;
  }
  else {
    if (myIsValid && myCanGrow) {
      myCurve.push_back(xy);
      myUnderliyingModel.addInPoint(xy);
      updateConnectedUpDownCurves<4>();
      if (myUnderliyingModel.stillGrowableAfterUpdate())
        return true;

      // we pop the last point we added
      // we mark this DLL segment as ungrowable to stop the recognition process
      myCurve.pop_back();
      myCanGrow = false;
    }
  }

  return false;
}

/**
  Comupte the freeman code between \a c1 and \a c2.
 */
template <typename DLL_Model>
unsigned int
Segment<DLL_Model>::computeFreemanCode(const Coordinates & c1,
                                       const Coordinates & c2) const
{
  /* the possible configurations of the vector between point c1 and point c2
     can be described with the freeman code depicted below

     3   2   1
       \ | /
        \|/
    4 ---X--- 0
        /|\
       / | \
     5   6   7
  */
  int diffX = static_cast<int>(c2.first  - c1.first );
  int diffY = static_cast<int>(c2.second - c1.second);

  for (size_t i = 0; i < 8; ++i)
    if (diffX == xFromFreeman[i] && diffY == yFromFreeman[i])
      return i;

  // should never go there
  throw;
}


/**
  Set as forbidden points the background N-neighbors of the curve (we don't take
  into account the first and last point of the curve durign this process in
  order not to have to remove points from the forbidden sets).
 */
template <typename DLL_Model>
template <int N>
void Segment<DLL_Model>::updateConnectedUpDownCurves()
{
  size_t size = myCurve.size();
  if (size < 3)
    //there is not enough points
    return;

  // we work with the 3 last points of the curve
  const Coordinates & lastPoint = myCurve[--size];
  const Coordinates & currentPoint = myCurve[--size];
  const Coordinates & previousPoint = myCurve[--size];

  unsigned int freemanEnter = computeFreemanCode(previousPoint, currentPoint);
  unsigned int freemanExit = computeFreemanCode(currentPoint, lastPoint);

  if ((freemanExit + 8 - freemanEnter) % 4 == 0 &&
      freemanExit != freemanEnter) {
    // Stationnary point: the best solution is to put a common point in
    // Up and Down which makes stop the GJK_nD algorithm
    myDownNeighbors.push_back(lastPoint);
    myUnderliyingModel.addDownPoint(lastPoint);
    myUpNeighbors.push_back(lastPoint);
    myUnderliyingModel.addUpPoint(lastPoint);

    return;
  }

  Coordinates coord;
  // update myDownNeighbors first
  size_t setSize = myDownNeighbors.size();
  for (unsigned int i = (freemanEnter + 5) % 8;
       i != freemanExit; i = (i + 1) % 8)
    if (N == 8 || i % 2 == 0) {
      coord = Coordinates(currentPoint.first  + xFromFreeman[i],
                          currentPoint.second + yFromFreeman[i]);
      if (setSize == 0 ||
          (coord != myDownNeighbors.back() &&
           (setSize == 1 || coord != myDownNeighbors[setSize - 2]))) {
        myDownNeighbors.push_back(coord);
        ++setSize;
        myUnderliyingModel.addDownPoint(coord);
      }
  }

  // and now update myUpNeighbors
  setSize = myUpNeighbors.size();
  for (unsigned int i = (freemanEnter + 3) % 8;
       i != freemanExit; i = (i + 7) % 8)
    if (N == 8 || i % 2 == 0) {
      coord = Coordinates(currentPoint.first  + xFromFreeman[i],
                          currentPoint.second + yFromFreeman[i]);
      if (setSize == 0 ||
          (coord != myUpNeighbors.back() &&
           (setSize == 1 || coord != myUpNeighbors[setSize - 2]))) {
        myUpNeighbors.push_back(coord);
        ++setSize;
        myUnderliyingModel.addUpPoint(coord);
      }
    }
}


template <typename DLL_Model>
std::ostream & operator<<(std::ostream & out, const Segment<DLL_Model> & dll)
{
  if (dll.isValid())
    out << dll.myUnderliyingModel;
  else
    out << "Non valid DLL";

  return out;
}

} // namespace DLL

#endif // DLL_SEGMENT_HPP
