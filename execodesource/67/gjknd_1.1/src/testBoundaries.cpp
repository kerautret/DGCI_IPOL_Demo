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
#include "utils.hpp"


using namespace std;

int main(int argc, char *argv[])
{
  // Command-line parsing ------------------------------------------------------
  std::string inputFile;
  std::string outputFile;
  bool blackBackground;

  try {
    TCLAP::CmdLine cmd("Extract the boundaries of a binary image", ' ', "1.0");
    TCLAP::UnlabeledValueArg<std::string> inputArg("input",
                                                   "The input file (PNG image)",
                                                   true, "", "file");
    TCLAP::ValueArg<std::string> outputArg("o", "output",
                                           "The output file",
                                           false, "", "file");
    TCLAP::SwitchArg bgArg("b", "black-background",
                           "Specify that the image has a very dark background",
                           false);
    cmd.add(inputArg);
    cmd.add(outputArg);
    cmd.add(bgArg);
    cmd.parse(argc, argv);

    inputFile = inputArg.getValue();
    outputFile = outputArg.getValue();
    blackBackground = bgArg.getValue();
  }
  catch (TCLAP::ArgException & e) {
    std::cerr << "Error: " << e.error() << " for arg " << e.argId()
              << std::endl;
    exit(EXIT_FAILURE);
  }
  // End of command-line parsing -----------------------------------------------

  if (outputFile == "")
    outputFile = "out_" + inputFile;

  png::image<png::gray_pixel> image(inputFile);
  Utils::otsuThresholding(image);
  if (!blackBackground)
    Utils::binaryImageToNegative(image);

  typedef Utils::BoundariesExtractor::Curve Curve;
  typedef Utils::BoundariesExtractor::Coordinates Coord;
  Utils::BoundariesExtractor be;

  std::vector<Curve> contours = be.extractBoundaries(image);

  const png::rgb_pixel pixelColors[] = {
    // redPixel
    png::rgb_pixel(255,0,0),
    // greenPixel
    png::rgb_pixel(0,255,0),
    // bluePixel
    png::rgb_pixel(0,0,255),
    // yellowPixel
    png::rgb_pixel(255,255,0)
  };

  unsigned int colorIndex = 0;
  png::image<png::rgb_pixel> output(image.get_width(), image.get_height());
  if (!blackBackground)
    Utils::fillImage(output, png::rgb_pixel(255,255,255));

  std::cout << "Contour number: " << contours.size() << std::endl;
  for (std::vector<Curve>::const_iterator curvesItor = contours.begin();
       curvesItor != contours.end(); ++curvesItor) {
    for(Curve::const_iterator coordItor = curvesItor->begin();
        coordItor != curvesItor->end();
        ++coordItor) {
      output.set_pixel(coordItor->first, coordItor->second,
                       pixelColors[colorIndex]);
    }
    colorIndex = (colorIndex + 1) % 4;
  }

  output.write(outputFile);

  return EXIT_SUCCESS;
}