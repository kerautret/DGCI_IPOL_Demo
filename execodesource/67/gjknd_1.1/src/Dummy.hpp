#ifndef DLL_DUMMY_HPP
#define DLL_DUMMY_HPP

#include <iostream>

/// \cond TEST
namespace DLL {

/**
  \class Dummy
  \brief A DLL model to represent nothing but an always growable DLL.

  The Dummy class reprensents DLL that have no analytical representation, but
  can always grow.
 */
class Dummy
{
private:
  typedef std::pair<size_t, size_t>   Coordinates;

public:
  /**
    Simulate an addition of a point to the set that has to lie inside the DLL.
   */
  void addInPoint  (const Coordinates &) { }

  /**
    Simulate an addition of a point to the set that has to lie on the negative
    side outside of the DLL.
   */
  void addDownPoint(const Coordinates &) { }

  /**
    Simulate an addition of a point to the set that has to lie on the positive
    side outside of the DLL.
   */
  void addUpPoint  (const Coordinates &) { }

  /**
    Do the recognition process with the current sets of points and returns true
    if the DLL is still growable after we've added points. If it returns false,
    the DLL is not growable anymore and its reliable characteristics are the
    ones we got from the last call to stillGrowableAfterUpdate() that returned
    true.
    */
  bool stillGrowableAfterUpdate() { return true;}

  /**
    Print the characteristics of the dummy DLL (i.e. nothing) on the \a out
    stream.
   */
  friend std::ostream & operator<<(std::ostream & out, const Dummy &)
  {
    return out;
  }

};

} // namespace DLL
/// \endcond

#endif // DLL_DUMMY_HPP
