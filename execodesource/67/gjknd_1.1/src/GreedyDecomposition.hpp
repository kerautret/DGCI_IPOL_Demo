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

#ifndef UTILS_GREEDY_DECOMPOSITION_HPP
#define UTILS_GREEDY_DECOMPOSITION_HPP

#include <vector>
#include <utility>

namespace Utils {

/**
  \class GreedyDecomposition
  \brief This class implements a greedy algorithm to decompose a digital curve
         into segments.

  The GreedyDecomposition is a templated class that decompose a digital curve
  into segments according to the given Segment model parameter. We start
  fom the first point of the curve and add points to the segment while it is
  still growable. If the set of points is not a valid segment anymore (wrt.
  Segment) we stop the recognition process and start a new segment with
  the next point of the curve, and so on until the whole curve has been treated.

  \tparam Segment is a concept that has to be default constructible and
                  copyable and provides:

  - a \b Coordinates type that defines the 2D coordinates of the pixels,
  - a \b Curve type that is an ordered set of Coordinates to define a curve,
  - a <b>bool addPoint(Coordinates)</b> function that adds a point to the
    segment and returns true if the segment is still growable,
  - a <b>bool isValid()</b> function that tells whether the segment is a
    valid one, i.e. if we can rely on its analytical representation,
  - a <b>Curve getCurve()</b> fonction that returns the underlying curve the
    segment represents,
  - a \b <tt><<</tt> operator that prints the characteristics of the segment on
    an output stream.

  \code
  ...
  Curve curve;
  // initialize the curve (e.g. with a BoundariesExtractor)

  typedef DLL::Segment<DLL::Circle>   DLLSegment;
  typedef std::vector<DLLSegment>     DLLList;

  Utils::GreedyDecomposition<DLLSegment> decompositor;

  DLLList circleDlls = decompositor.decomposeCurve(curve);

  // print the characteristics of each DLL segments of the curve
  for (DLLList::const_iterator dllsItor = circleDlls.begin();
       dllsItor != circleDlls.end(); ++dllsItor) {
    std::cout << "DLL: " << *dllsItor << std::endl;
  }
  \endcode
 */
template <typename Segment>
class GreedyDecomposition
{
public:
  /// The coordinates type of each pixel of the contour
  typedef typename Segment::Coordinates   Coordinates;
  /// The type of a contour
  typedef typename Segment::Curve         Curve;

public:
  /**
    Create a new GreedyDecomposition to decompose the \a curve into segments
    (wrt. Segment).
   */
  GreedyDecomposition(const Curve * curve = 0) : myCurve(curve) {}
  ~GreedyDecomposition() {}

  /**
    Associate the \a curve to this GreedyDecomposition instance.
   */
  void setCurve(const Curve * curve) { myCurve = curve; }

  /**
    Decompose the curve associated to this instance of GreedyDecomposition.
    \return the partiton of the curve as a set of segments.
   */
  std::vector<Segment> decomposeCurve();

  /**
    Decompose the \c curve into segments.
    \return the partiton of the curve as a set of segments.
   */
  std::vector<Segment> decomposeCurve(const Curve & curve)
  {
    std::vector<Segment> result;
    const Curve * myCurveBak = myCurve;
    setCurve(&curve);
    result = decomposeCurve();
    myCurve = myCurveBak;
    return result;
  }

private:
  GreedyDecomposition(const GreedyDecomposition &);
  GreedyDecomposition & operator=(const GreedyDecomposition &);

private:
  const Curve * myCurve;
};


template <typename Segment>
std::vector<Segment> GreedyDecomposition<Segment>::decomposeCurve()
{
  std::vector<Segment> result;

  Segment segment;
  typename Curve::const_iterator pixelItor = myCurve->begin();
  while (pixelItor != myCurve->end()) {
    if (!segment.addPoint(*pixelItor)) {
      result.push_back(segment);
      segment = Segment();
      continue;
    }
    ++pixelItor;
  }
  if (segment.isValid())
    result.push_back(segment);

  return result;
}

} // namespace Utils

#endif // UTILS_GREEDY_DECOMPOSITION_HPP
