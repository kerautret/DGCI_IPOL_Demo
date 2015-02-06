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

#include "BoundariesExtractor.hpp"
#include <queue>
#include "Array2D.hpp"

namespace Utils {

/*
 Initialization:
  We start by creating two arrays for the vertical and horizontal edges.
  verticalEdge(i,j) (resp. horizontalEdge(i,j)) is 1 if the pixels image(i,j)
  and image(i+1,j) (resp. image(i,j) and image(i,j+1)) have a different color
  We then consider all the edges with neighboring pixels of different colors.
  We put their values at -1 (otherwise it's 0).

 Body:
  For each -1 founded in the array we compute the curve of this edge with the
  routine trackNextEdge (reccursion).
  In order to have an ordered sequence of points, we start by the edges at the
  boundary of the image and then, we treat the interior edges.
*/
std::vector<BoundariesExtractor::Curve> BoundariesExtractor::extractBoundaries()
{
  std::vector<Curve> result;

  if (myImage) {
    // we initialize the edge arrays for the contour extraction
    verticalEdge   = Array2D<int>(myImage->get_width() - 1,
                                  myImage->get_height());
    horizontalEdge = Array2D<int>(myImage->get_width(),
                                  myImage->get_height() - 1);

    for (size_t i = 0; i < verticalEdge.width(); ++i)
      for (size_t j = 0; j < verticalEdge.height(); ++j) {
        verticalEdge(i,j) = 0;
        if (myImage->get_pixel(i,j) != myImage->get_pixel(i+1,j))
          verticalEdge(i,j) = -1;
      }

    for (size_t i = 0; i < horizontalEdge.width(); ++i)
      for (size_t j = 0; j < horizontalEdge.height(); ++j) {
        horizontalEdge(i,j) = 0;
        if (myImage->get_pixel(i,j) != myImage->get_pixel(i,j+1))
          horizontalEdge(i,j) = -1;
      }

    // We shall compute the connected components of this graph structure
    // registered in the two edges arrays. We investigate the vertical edges to
    // start the curve extraction. The edges that have already been extracted
    // take negative values... the second curve has values -2...beware of the
    // possible ambiguity
    // 01
    // 10
    int color = 0;

    Curve currentCurve;
    // we start with the vertical edges on the horizontal boundaries
    for (size_t i = 0; i < verticalEdge.width(); ++i) {
      if (verticalEdge(i,0) == -1) {
        ++color;
        currentCurve.clear();
        trackNextEgde(Vertical, i, 0, color,
                       currentCurve);
        result.push_back(currentCurve);
      }
      if (verticalEdge(i,verticalEdge.height()-1) == -1) {
        ++color;
        currentCurve.clear();
        trackNextEgde(Vertical, i, verticalEdge.height()-1, color,
                       currentCurve);
        result.push_back(currentCurve);
      }
    }

    // we continue with the horizontal edges on the vertical boundaries
    for (size_t j = 0; j < horizontalEdge.height(); ++j) {
      if (horizontalEdge(0,j) == -1) {
        ++color;
        currentCurve.clear();
        trackNextEgde(Horizontal, 0, j, color,
                       currentCurve);
        result.push_back(currentCurve);
      }
      if (horizontalEdge(horizontalEdge.width()-1,j) == -1) {
        ++color;
        currentCurve.clear();
        trackNextEgde(Horizontal, horizontalEdge.width()-1, j, color,
                       currentCurve);
        result.push_back(currentCurve);
      }
    }

    // we eventually treat the rest of the image
    for (size_t i = 1; i< verticalEdge.width(); ++i)
      for (size_t j = 1; j < horizontalEdge.height(); ++j) {
        if (verticalEdge(i,j) == -1) {
          ++color;
          currentCurve.clear();
          trackNextEgde(Vertical, i, j, color,
                         currentCurve);
          // close the curve if needed
          if (currentCurve.front() != currentCurve.back())
            currentCurve.push_back(currentCurve.front());

          result.push_back(currentCurve);
        }

        if (horizontalEdge(i,j) == -1) {
          ++color;
          currentCurve.clear();
          trackNextEgde(Horizontal, i, j, color,
                         currentCurve);
          // close the curve if needed
          if (currentCurve.front() != currentCurve.back())
            currentCurve.push_back(currentCurve.front());

          result.push_back(currentCurve);
        }
      }
  }
  return result;
}


/**
  This function color a single 4-connected component that contains the starting
  pixel \a coords with the value \a label. We use a classical BFS of the
  4-neighborhood of the pixel.
 */
void BoundariesExtractor::color4ConnectedComponent(Coordinates coords,
                                                   png::gray_pixel label,
                                                   const Image & inputImage,
                                                   Image & outputImage)
{
  const size_t sizeX = inputImage.get_width();
  const size_t sizeY = inputImage.get_height();

  std::queue<Coordinates> toVisit;
  outputImage.set_pixel(coords.first, coords.second, label);
  toVisit.push(coords);

  while (!toVisit.empty()) {
    size_t i = toVisit.front().first;
    size_t j = toVisit.front().second;
    toVisit.pop();

    // west neighbor
    if (i > 0) {
      if (inputImage.get_pixel(i-1, j) != 0 &&
          outputImage.get_pixel(i-1, j) == 0) {
        outputImage.set_pixel(i-1, j, label);
        toVisit.push(Coordinates(i-1, j));
      }
    }
    // south neighbor
    if (j > 0) {
      if (inputImage.get_pixel(i, j-1) != 0 &&
          outputImage.get_pixel(i, j-1) == 0) {
        outputImage.set_pixel(i, j-1, label);
        toVisit.push(Coordinates(i, j-1));
      }
    }
    // east neighbor
    if (i < sizeX - 1) {
      if (inputImage.get_pixel(i+1, j) != 0 &&
          outputImage.get_pixel(i+1, j) == 0) {
        outputImage.set_pixel(i+1, j, label);
        toVisit.push(Coordinates(i+1, j));
      }
    }
    // north neighbor
    if (j < sizeY - 1) {
      if (inputImage.get_pixel(i, j+1) != 0 &&
          outputImage.get_pixel(i, j+1) == 0) {
        outputImage.set_pixel(i, j+1, label);
        toVisit.push(Coordinates(i, j+1));
      }
    }
  }
}


/**
  Compute an new image where all the 4-connected components of \a inputImage
  are labeled with a different value.
 */
BoundariesExtractor::Image
BoundariesExtractor::color4ConnectedComponents(const Image & inputImage)
{
  Image outputImage(inputImage.get_width(), inputImage.get_height());
  png::gray_pixel label = 0;

  for (size_t i = 0; i < inputImage.get_width(); ++i) {
    for (size_t j = 0; j < inputImage.get_height(); ++j) {
      if (inputImage.get_pixel(i, j) != 0 && outputImage.get_pixel(i, j) == 0) {
        ++label;
        color4ConnectedComponent(Coordinates(i, j), label, inputImage,
                                 outputImage);
      }
    }
  }

  return outputImage;
}


/**
  This function is the core routine of the contour extraction. It extracts the
  edges of the curve containing a given edge. We have the edge (horizontal or
  vertical depending on the \a direction parameter) of coordinate (\a i, \a j)
  and we want to compute the next edges in both directions (clockwise and
  counterclockwise). We mark the edge during the traversal by updating its
  value from -1 to -2 before doing the second direction and, last, to \a color.
 */
void BoundariesExtractor::trackNextEgde(Direction direction,
                                         size_t i, size_t j, int color,
                                         Curve & result)
{
  // we follow the curve in the trignometric direction
  if (direction == Vertical) {
    if (verticalEdge(i,j) == -1) {
      verticalEdge(i,j) = -2;
      if (myImage->get_pixel(i,j) > 0) {
        // we determine if the new point (i,j) should be added in the list
        if (result.empty() || Coordinates(i,j) != result.back()) {
          result.push_back(Coordinates(i,j));
        }

        // we search the next edge
        if (j < horizontalEdge.height()) {
          if (horizontalEdge(i,j) != 0) {
            if (horizontalEdge(i,j) == -1) {
              trackNextEgde(Horizontal, i, j, color, result);
            }
          }
          else {
            if (verticalEdge(i,j+1) != 0) {
              if (verticalEdge(i,j+1) == -1) {
                trackNextEgde(Vertical, i, j+1, color, result);
              }
            }
            else {
              if (i < horizontalEdge.width() - 1) {
                if (horizontalEdge(i+1,j) == -1) {
                  trackNextEgde(Horizontal, i+1, j, color, result);
                }
              }
            }
          }
        }
      }
      else {
        // we determine if the new point (i+1,j) should be added in the list
        if (result.empty() || Coordinates(i+1,j) != result.back()) {
          result.push_back(Coordinates(i+1,j));
        }

        // we search the next edges
        if (j > 0) {
          if (horizontalEdge(i+1,j-1) != 0) {
            if (horizontalEdge(i+1,j-1) == -1) {
              trackNextEgde(Horizontal, i+1, j-1, color, result);
            }
          }
          else {
            if (verticalEdge(i,j-1) != 0) {
              if (verticalEdge(i,j-1) == -1) {
                trackNextEgde(Vertical, i, j-1, color, result);
              }
            }
            else {
              if (horizontalEdge(i,j-1) == -1) {
                trackNextEgde(Horizontal, i, j-1, color, result);
              }
            }
          }
        }
      }
    }
  }
  if (direction == Horizontal) {
    if (horizontalEdge(i,j) == -1) {
      horizontalEdge(i,j) = -2;
      if (myImage->get_pixel(i,j) > 0) {
        // we determine if the new point (i,j) should be added in the list
        if (result.empty() || Coordinates(i,j) != result.back()) {
          result.push_back(Coordinates(i,j));
        }

        if (i > 0) {
          if (verticalEdge(i-1,j) != 0) {
            if (verticalEdge(i-1,j) == -1) {
              trackNextEgde(Vertical, i-1, j, color, result);
            }
          }
          else {
            if (horizontalEdge(i-1,j) != 0) {
              if (horizontalEdge(i-1,j) == -1) {
                trackNextEgde(Horizontal, i-1, j, color, result);
              }
            }
            else {
              if (j < verticalEdge.height() - 1) {
                if (verticalEdge(i-1,j+1) == -1) {
                  trackNextEgde(Vertical, i-1, j+1, color, result);
                }
              }
            }
          }
        }
      }
      else {
        // we add the new point (i,j+1) or not...
        if (result.empty() || Coordinates(i,j+1) != result.back()) {
          result.push_back(Coordinates(i,j+1));
        }

        if (i < verticalEdge.width()) {
          if (verticalEdge(i,j+1) != 0) {
            if (verticalEdge(i,j+1) == -1) {
              trackNextEgde(Vertical, i, j+1, color, result);
            }
          }
          else {
            if (horizontalEdge(i+1,j) != 0) {
              if (horizontalEdge(i+1,j) == -1) {
                trackNextEgde(Horizontal, i+1, j, color, result);
              }
            }
            else {
              if (verticalEdge(i,j) == -1) {
                trackNextEgde(Vertical, i, j, color, result);
              }
            }
          }
        }
      }
    }
  }

  // now, we follow the curve in the antitrigonometric direction
  if (direction == Horizontal) {
    if (horizontalEdge(i,j) == -2 || horizontalEdge(i,j) == -1) {
      horizontalEdge(i,j) = color;
      if (myImage->get_pixel(i,j) > 0) {
        if (i < verticalEdge.width()) {
          if (verticalEdge(i,j) != 0) {
            if (verticalEdge(i,j) == -1) {
              trackNextEgde(Vertical, i, j, color, result);
            }
          }
          else {
            if (horizontalEdge(i+1,j) != 0) {
              if (horizontalEdge(i+1,j) == -1) {
                trackNextEgde(Horizontal, i+1, j, color, result);
              }
            }
            else {
              if (j < verticalEdge.height() - 1) {
                if (verticalEdge(i,j+1) == -1) {
                  trackNextEgde(Vertical, i, j+1, color, result);
                }
              }
            }
          }
        }
      }
      else {
        if (i > 0) {
          if (verticalEdge(i-1,j+1) != 0) {
            if (verticalEdge(i-1,j+1) == -1) {
              trackNextEgde(Vertical, i-1, j+1, color, result);
            }
          }
          else {
            if (horizontalEdge(i-1,j) != 0) {
              if (horizontalEdge(i-1,j) == -1) {
                trackNextEgde(Horizontal, i-1, j, color, result);
              }
            }
            else {
              if (verticalEdge(i-1,j) == -1) {
                trackNextEgde(Vertical, i-1, j, color, result);
              }
            }
          }
        }
      }
    }
  }
  if (direction == Vertical) {
    if (verticalEdge(i,j) == -2 || verticalEdge(i,j) == -1) {
      verticalEdge(i,j)= color;
      if (myImage->get_pixel(i,j) > 0) {
        if (j > 0) {
          if (horizontalEdge(i,j-1) != 0) {
            if (horizontalEdge(i,j-1) == -1) {
              trackNextEgde(Horizontal, i, j-1, color, result);
            }
          }
          else {
            if (verticalEdge(i,j-1) != 0) {
              if (verticalEdge(i,j-1) == -1) {
                trackNextEgde(Vertical, i, j-1, color, result);
              }
            }
            if (i < horizontalEdge.width() - 1) {
              if (horizontalEdge(i+1,j-1) == -1) {
                trackNextEgde(Horizontal, i+1, j-1, color, result);
              }
            }
          }
        }
      }
      else {
        if (j < horizontalEdge.height()) {
          if (horizontalEdge(i+1,j) != 0) {
            if (horizontalEdge(i+1,j) == -1) {
              trackNextEgde(Horizontal, i+1, j, color, result);
            }
          }
          else{
            if (verticalEdge(i,j+1) != 0) {
              if (verticalEdge(i,j+1) == -1) {
                trackNextEgde(Vertical, i, j+1, color, result);
              }
            }
            else{
              if (horizontalEdge(i,j) == -1) {
                trackNextEgde(Horizontal, i, j, color, result);
              }
            }
          }
        }
      }
    }
  }
}

} // namespace Utils
