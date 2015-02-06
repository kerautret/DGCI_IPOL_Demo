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

#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include "png++/png.hpp"

namespace Utils {

/**
  \file utils.hpp
  \brief Contains usefull helper functions to work with PNG images.
 */


/**
  \brief A helper function to compute the square of a \a value.
  \param value rtes
  \tparam T the number type of the value (the cost of a copy of T should be low
            because the parameter is passed by value).
 */
template <typename T>
inline T square(T value)
{
  return value * value;
}


/**
  \brief A helper function to get the sign of a number.
  \tparam T the number type of the value (the cost of a copy of T should be low
            because the parameter is passed by value).
  \return -1 when \a value is negative,
  \return  0 when \a value is null,
  \return  1 when \a value is positive.
 */
template <typename T>
int sign(T value)
{
  return (T(0) < value) - (value < T(0));
}


/**
  \brief A conversion function from any type T that have an << operator
         to std::string
 */
template<typename T>
inline std::string toString(T value)
{
  std::ostringstream oss;
  oss << value;
  return oss.str();
}


/**
  \brief Binarization of the \a image by Otsu's method based on maximization of
         inter-class variance.
  \see http://cis.k.hosei.ac.jp/~wakahara/otsu_th.c
 */
void otsuThresholding(png::image<png::gray_pixel> & image);


/**
  \brief Transform the binary \a image into its negative counterpart.
 */
void binaryImageToNegative(png::image<png::gray_pixel> & image);


/**
  \brief Fill a PNG \a image with a given \a pixel.
 */
template <typename pixel_type>
void fillImage(png::image<pixel_type> & image, const pixel_type & pixel)
{
  for (size_t i = 0; i < image.get_width(); ++i)
    for (size_t j = 0; j < image.get_height(); ++j)
      image.set_pixel(i,j, pixel);
}

} // namespace Utils

#endif // UTILS_HPP
