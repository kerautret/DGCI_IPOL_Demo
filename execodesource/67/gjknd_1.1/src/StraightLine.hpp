#ifndef DLL_STRAIGHTLINE_HPP
#define DLL_STRAIGHTLINE_HPP

#include <iostream>
#include "GJK_nD.hpp"

namespace DLL {

/**
  \class StraightLine
  \brief A DLL model to represent digital straight lines.

  The StraightLine class reprensents DLL that have an analytical representation
  of the form: \htmlonly h &le; a.x + b.y &le; H\endhtmlonly.
 */
class StraightLine
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
    Print the characteristics of the \a line on the \a out stream.
   */
  friend std::ostream & operator<<(std::ostream & out,
                                   const StraightLine & line);

private:
  void addPointToSet(const Coordinates & xy, std::vector<Point> & set);

private:
  std::vector<Point> setIn;
  std::vector<Point> setDown;
  std::vector<Point> setUp;

  double a, b, lower_bound, upper_bound;
};

} // namespace DLL

#endif // DLL_STRAIGHTLINE_HPP
