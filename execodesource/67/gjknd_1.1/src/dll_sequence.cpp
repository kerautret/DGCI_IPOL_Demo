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

#include <cstdlib>
#include <string>
#include <iostream>
#include "tclap/CmdLine.h"
#include "png++/png.hpp"
#include "BoundariesExtractor.hpp"
#include "GreedyDecomposition.hpp"
#include "Segment.hpp"
#include "StraightLine.hpp"
#include "Circle.hpp"
#include "Conic.hpp"
#include "utils.hpp"

bool blackBackground;

/**
  \brief Return true if pixels p1 and p2 are the same color and false othewise.
 */
bool samePixels(const png::rgb_pixel & p1, const png::rgb_pixel & p2)
{
  return p1.red   == p2.red   &&
         p1.green == p2.green &&
         p1.blue  == p2.blue;
}


/**
  \brief Fill the non-background pixels of a PNG \a image with the foreground
         color.
 */
template <typename pixel_type>
void fillNonBgWithFg(png::image<pixel_type> & image)
{
  png::rgb_pixel bgColor, fgColor;
  if (blackBackground) {
    bgColor = png::rgb_pixel(0, 0, 0);
    fgColor = png::rgb_pixel(255, 255, 255);
  }
  else {
    bgColor = png::rgb_pixel(255, 255, 255);
    fgColor = png::rgb_pixel(0, 0, 0);
  }

  for (size_t i = 0; i < image.get_width(); ++i)
    for (size_t j = 0; j < image.get_height(); ++j)
      if (!samePixels(image.get_pixel(i,j), bgColor))
        image.set_pixel(i,j, fgColor);
}


template <typename DLL_Type, typename Curve>
void generateDecompositionSequence(const std::vector<Curve> & contours,
                                   png::image<png::rgb_pixel> & output,
                                   const std::string & filename)
{
  // Color map for the DLL
  const png::rgb_pixel pixelColors[4] = {
    // redPixel
    png::rgb_pixel(255,0,0),
    // greenPixel
    png::rgb_pixel(0,255,0),
    // bluePixel
    png::rgb_pixel(0,0,255)
  };

  typedef typename DLL::Segment<DLL_Type>   DLLSegment;
  Utils::GreedyDecomposition<DLLSegment> decompositor;

  int numCurve = 0;
  for (typename std::vector<Curve>::const_iterator contourItor = contours.begin();
       contourItor != contours.end(); ++contourItor) {
    // decompose the current contour
    typename std::vector<DLLSegment> dlls = decompositor.decomposeCurve(*contourItor);
    // color each DLL segment into the output image
    // and display the points coordinates of the DLL segment on the console
    for (typename std::vector<DLLSegment>::const_iterator dllItor = dlls.begin();
         dllItor != dlls.end(); ++dllItor) {
      // color the current DLL segment's inliers
      const Curve & curve = dllItor->getCurve();
      for(typename Curve::const_iterator coordItor = curve.begin();
          coordItor != curve.end();
          ++coordItor)
        output.set_pixel(coordItor->first, coordItor->second,
                         pixelColors[2]);

      // color the current DLL segment's positive outliers
      const Curve & upCurve = dllItor->getUpCurve();
      for(typename Curve::const_iterator coordItor = upCurve.begin();
          coordItor != upCurve.end();
          ++coordItor)
        output.set_pixel(coordItor->first, coordItor->second,
                         pixelColors[0]);

      // color the current DLL segment's negative outliers
      const Curve & downCurve = dllItor->getDownCurve();
      for(typename Curve::const_iterator coordItor = downCurve.begin();
          coordItor != downCurve.end();
          ++coordItor)
        output.set_pixel(coordItor->first, coordItor->second,
                         pixelColors[1]);

      output.write(Utils::toString(numCurve) + "_" + filename);
      ++numCurve;
      fillNonBgWithFg(output);
    }
  }
}


int main(int argc, char *argv[])
{
  // Command-line parsing ------------------------------------------------------
  std::string inputFile;
  std::string outputFile;
  std::vector<std::string> models;

  try {
    TCLAP::CmdLine cmd("Creates a set of PNG images that reprensents the steps "
                       "of the decomposition process of the contours of an "
                       "input PNG image into DLL segments.", ' ', "1.0");
    TCLAP::UnlabeledValueArg<std::string> inputArg(
          "input", "The input PNG image.", true, "", "file"
    );
    TCLAP::SwitchArg bgArg(
          "b", "black-background",
          "Specify that the image has a very dark background.", false
    );
    TCLAP::MultiArg<std::string> dllArg(
          "d", "dll-model",
          "The underlying DLL model used for the decomposition.", false,
          "StraightLine | Circle | Conic"
    );

    cmd.add(inputArg);
    cmd.add(bgArg);
    cmd.add(dllArg);
    cmd.parse(argc, argv);

    inputFile = inputArg.getValue();
    blackBackground = bgArg.getValue();
    models = dllArg.getValue();
  }
  catch (TCLAP::ArgException & e) {
    std::cerr << "Error: " << e.error() << " for arg " << e.argId()
              << std::endl;
    exit(EXIT_FAILURE);
  }
  // End of command-line parsing -----------------------------------------------

  outputFile = "out_";
  size_t startPos = inputFile.find_last_of("/\\");
  if (startPos == std::string::npos)
    outputFile += inputFile;
  else
    outputFile += inputFile.substr(startPos + 1);

  png::image<png::gray_pixel> image(inputFile);

  // Binarization
  Utils::otsuThresholding(image);
  if (!blackBackground)
    Utils::binaryImageToNegative(image);

  // Contours extraction
  typedef Utils::BoundariesExtractor::Curve Curve;
  typedef Utils::BoundariesExtractor::Coordinates Coord;
  Utils::BoundariesExtractor be;
  std::vector<Curve> contours = be.extractBoundaries(image);

  png::image<png::rgb_pixel> output(image.get_width(), image.get_height());

  if (models.empty()) {
    models.push_back("StraightLine");
    models.push_back("Circle");
    models.push_back("Conic");
  }
  // Decompose into DLLs and save the result
  for (size_t i = 0; i < models.size(); ++i) {
    if (!blackBackground)
      Utils::fillImage(output, png::rgb_pixel(255,255,255));

    if (models[i] == "StraightLine") {
      generateDecompositionSequence<DLL::StraightLine>(
            contours, output, "StraightLine_" + outputFile);
    }
    else if (models[i] == "Circle") {
      generateDecompositionSequence<DLL::Circle>(
            contours, output, "Circle_" + outputFile);
    }
    else if (models[i] == "Conic") {
      generateDecompositionSequence<DLL::Conic>(
            contours, output, "Conic_" + outputFile);
    }
    else
      std::cerr << "Unknown " << models[i] << " DLL model" << std::endl;
  }

  return EXIT_SUCCESS;
}
