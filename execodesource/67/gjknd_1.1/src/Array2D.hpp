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

#ifndef UTILS_ARRAY2D_HPP
#define UTILS_ARRAY2D_HPP

#include <algorithm>

namespace Utils {

/**
  \class Array2D
  \brief A minimalist but handy templated class to manipulate 2D arrays
         of values.

  Array2D allows us to create a \a m x \a n array and acces its elements
  through the () operator.
  \code
  Array2D<int> tab(3,7);

  std::cout << "Value at index (1,4): " << tab(1,4) << std::endl;
  tab(0,5) = 42;
  \endcode
 */
template <typename T>
class Array2D
{
public:
  /**
    Construct an empty array.
   */
  Array2D()
    : myWidth (0), myHeight(0), myArray (0)
  {}

  /**
    Construct a \a width x \a height array.
   */
  Array2D(size_t width, size_t height)
    : myWidth (width), myHeight(height), myArray (0)
  {
    if(width > 0 && height > 0)
      myArray = new T[width * height];
  }

  /**
    Create a copy of \a srcArray.
   */
  Array2D(const Array2D<T> & srcArray)
    : myWidth(srcArray.myWidth), myHeight(srcArray.myHeight), myArray(0)
  {
    if(myWidth > 0 && myHeight > 0) {
      myArray = new T[myWidth * myHeight];
      std::copy(srcArray.myArray, srcArray.myArray + myWidth * myHeight,
                myArray);
    }
  }

  /**
    Destroy the array
   */
  ~Array2D()
  {
    if (myArray)
      delete[] myArray;
  }

  /**
    Copy the content of \a srcArray into this array
   */
  Array2D & operator=(const Array2D<T> & srcArray)
  {
    Array2D copy(srcArray);

    myWidth = copy.myWidth;
    myHeight = copy.myHeight;
    std::swap(myArray, copy.myArray);

    return *this;
  }

  /**
    Return the contents of the array at position (\a x, \a y)
   */
  const T & operator () (size_t x, size_t y) const
  {
    return myArray[y * myWidth + x];
  }

  /**
    Return an l-value reference to the element at position (\a x, \a y).
    This element can thus be modified.
   */
  T & operator () (size_t x, size_t y)
  {
    return myArray[y * myWidth + x];
  }

  /**
    Return the width of the array.
   */
  size_t width() const { return myWidth; }

  /**
    Return the height of the array.
   */
  size_t height() const { return myHeight; }

private:
  size_t myWidth;
  size_t myHeight;
  T *    myArray;
};

} // namespace Utils

#endif // UTILS_ARRAY2D_HPP
