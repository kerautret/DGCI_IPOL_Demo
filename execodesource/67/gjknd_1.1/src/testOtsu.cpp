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
#include "utils.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  // Command-line parsing ------------------------------------------------------
  std::string inputFile;
  std::string outputFile;

  try {
    TCLAP::CmdLine cmd("Image binarization by using Otsu method", ' ', "1.0");
    TCLAP::UnlabeledValueArg<std::string> inputArg("input",
                                                   "The input file (PNG image)",
                                                   true, "", "file");
    TCLAP::ValueArg<std::string> outputArg("o", "output",
                                           "The output file",
                                           false, "", "file");
    cmd.add(inputArg);
    cmd.add(outputArg);
    cmd.parse(argc, argv);

    inputFile = inputArg.getValue();
    outputFile = outputArg.getValue();
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
  image.write(outputFile);

  return EXIT_SUCCESS;
}
