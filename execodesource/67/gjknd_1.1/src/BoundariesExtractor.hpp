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

#ifndef UTILS_BOUNDARIES_EXTRACTOR_HPP
#define UTILS_BOUNDARIES_EXTRACTOR_HPP

#include <vector>
#include <utility>
#include "Array2D.hpp"
#include "png++/png.hpp"

namespace Utils {

/**
  \class BoundariesExtractor
  \brief A utility class to extract the boundaries of a binary image.

  The BoundariesExtractor class expects a binary image with a black background
  (i.e. with a pixel value of 0) and a non-black foreground (i.e. pixels value
  that are different from 0 are considered as foreground). The boundaries of
  each 4-connected component of the image are extracted and returned as a set
  of Curve which are an ordered set of coordinates.

  \code
  ...
  png::image<png::gray_pixel> image(inputFile);

  typedef Utils::BoundariesExtractor::Curve   Curve;

  Utils::BoundariesExtractor be;
  // set the image to work with
  be.setImage(&image);
  // do the contour tracking
  std::vector<Curve> contours = be.extractBoundaries();

  // this 2 steps could be replaced by a call to
  // std::vector<Curve> contours = be.extractBoundaries(image);

  // print the pixels' coordinates of each contour curve
  for (std::vector<Curve>::const_iterator curveItor = contours.begin();
       curveItor != contours.end(); ++curveItor) {
    std::cout << "Curve: ";
    for(Curve::const_iterator coordItor = curveItor->begin();
        coordItor != curveItor->end(); ++coordItor) {
      std::cout << "(" << coordItor->first << "," << coordItor->second << ") ";
    }
    std::cout << std::endl;
  }
  \endcode
 */
class BoundariesExtractor
{
public:
  /// The coordinates type of each pixel of the contour
  typedef std::pair<size_t, size_t>     Coordinates;
  /// The type of a contour
  typedef std::vector<Coordinates>      Curve;
  /// The type of images to work with
  typedef png::image<png::gray_pixel>   Image;

public:
  /**
    Create a new BoundariesExtractor to extract the contour of each 4-connected
    component of the \a image.
   */
  BoundariesExtractor(const Image * image = 0) : myImage(image) {}
  ~BoundariesExtractor() {}

  /**
    Associate the \a image to this BoundariesExtractor instance.
   */
  void setImage(const Image * image) { myImage = image; }

  /**
    Extract the boundaries of the image associated to this instance of
    BoundariesExctractor.
    \return the different contours as a set of 8-connected curves.
   */
  std::vector<Curve> extractBoundaries();

  /**
    Extract the boundaries of the \a image.
    \return the different contours as a set of 8-connected curves
   */
  std::vector<Curve> extractBoundaries(const Image & image)
  {
    std::vector<Curve> result;
    const Image * myImageBak = myImage;
    setImage(&image);
    result = extractBoundaries();
    myImage = myImageBak;
    return result;
  }

private:
  enum Direction {Horizontal, Vertical};

  void color4ConnectedComponent(Coordinates coords, png::gray_pixel label,
                                const Image & inputImage,
                                Image & outputImage);

  Image color4ConnectedComponents(const Image & inputImage);

  void trackNextEgde(Direction direction, size_t i, size_t j, int color,
                     Curve & result);

private:
  BoundariesExtractor(const BoundariesExtractor &);
  BoundariesExtractor & operator=(const BoundariesExtractor &);

private:
  const Image * myImage;

  Utils::Array2D<int> verticalEdge;
  Utils::Array2D<int> horizontalEdge;
};

} // namespace Utils

#endif // UTILS_BOUNDARIES_EXTRACTOR_HPP
