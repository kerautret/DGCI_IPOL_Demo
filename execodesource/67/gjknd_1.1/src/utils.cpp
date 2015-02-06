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

#include "utils.hpp"
#include <vector>
#include <limits>

namespace Utils {

void otsuThresholding(png::image<png::gray_pixel> & image)
{
  const int numPixels = image.get_width() * image.get_height();
  const size_t maxPixelValue = static_cast<size_t>(
        std::numeric_limits<png::gray_pixel>::max());

  // histogram generation
  std::vector<unsigned int> histogram(maxPixelValue + 1, 0);
  for (size_t i = 0; i < image.get_width(); ++i)
    for (size_t j = 0; j < image.get_height(); ++j)
      ++histogram[image.get_pixel(i,j)];

  // computation of the probabilities of each density level
  std::vector<double> prob(maxPixelValue + 1);
  for (size_t i = 0; i <= maxPixelValue; ++i) {
    prob[i] = static_cast<double>(histogram[i]) / numPixels;
  }

  // omega & mu computation
  std::vector<double> omega(maxPixelValue + 1);
  std::vector<double> mu(maxPixelValue + 1);
  omega[0] = prob[0];
  mu[0] = 0.0;
  for (size_t i = 1; i <= maxPixelValue; ++i) {
    omega[i] = omega[i-1] + prob[i];
    mu[i] = mu[i-1] + i * prob[i];
  }

  // sigma maximization
  // sigma stands for inter-class variance
  // and determines optimal threshold value
  std::vector<double> sigma(maxPixelValue + 1);
  size_t threshold = 0;
  double max_sigma = 0.0;
  for (size_t i = 0; i <= maxPixelValue-1; ++i) {
    if (omega[i] != 0.0 && omega[i] != 1.0)
      sigma[i] = square(mu[maxPixelValue] * omega[i] - mu[i])
          / (omega[i] * (1.0 - omega[i]));
    else
      sigma[i] = 0.0;
    if (sigma[i] > max_sigma) {
      max_sigma = sigma[i];
      threshold = i;
    }
  }

  // binarization
  for (size_t i = 0; i < image.get_width(); ++i)
    for (size_t j = 0; j < image.get_height(); ++j)
      if (image.get_pixel(i,j) > threshold)
        image.set_pixel(i,j, std::numeric_limits<png::gray_pixel>::max());
      else
        image.set_pixel(i,j, 0);
}


void binaryImageToNegative(png::image<png::gray_pixel> & image)
{
  for (size_t i = 0; i < image.get_width(); ++i)
    for (size_t j = 0; j < image.get_height(); ++j) {
      image.set_pixel(i,j, (image.get_pixel(i,j) == 0 ?
                              std::numeric_limits<png::gray_pixel>::max() : 0));
    }
}

} // namespace Utils
