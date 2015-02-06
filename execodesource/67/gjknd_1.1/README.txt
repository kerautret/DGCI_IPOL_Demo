This program is written by Laurent Provot <provot.research@gmail.com> and
Yan Gerard <yan.gerard@u-clermont1.fr> under the terms of the GPLv3 license.


1. Overview:
------------

GJK_nD is an implementation of a variation of the GJK algorithm that allows
us to separate 3 sets of points with a couple of parallel hyperplanes.
We use this method to decompose the border of binary object into a set of
Digital Level Layers (DLL for short). A DLL is a new kind of digital primitive
that generalizes the digital analytical hyperplanes by considering non linear
underlying models (see the "Digital Level Layers for Digital Curve
Decomposition and Vectorization" IPOL page for a detailed description).


2. Contents:
-------------

The program is coded in C++ and divided into 3 parts:
- The core algortihm which is the GJKnD implementation (GJK_nD files),
- The DLL related classes (Segment, Circle, StraightLine, Ellipse, Conic) for
  the decomposition into DLL segments,
- Some image processing utilities (Array2D, BoundariesExtractor,
  GreedyDecomposition, utils)

The source code's archive also contains a version of:
- the Eigen library, for linear algebra (http://eigen.tuxfamily.org/),
- TCLAP, for command line parsing (http://tclap.sourceforge.net/)
- png++, a handy C++ wrapper for libpng
  (http://savannah.nongnu.org/projects/pngpp/)

These are pure template libraries that require no external dependencies (expect
png++ which requires libpng, but this library is available on IPOL servers).


3. Compiling:
-------------

The program can easily be compiled with CMake. Just create a build directory
and cd into (for out-of-source compilation). Then run:

cmake ..
make

The executable are created in the src directory.


4. Program usage:
-----------------

The executable, called dll_decomposition, takes two parameters:
1) the underlying DLL models to be used during the decomposition process,
2) a PNG image to work on.

We provide an implementation for three models: StraightLine, Circle, Conic.
You can specify which model is used for the decomposition through the
--dll-model (-d) command line option. If no model is given, a decomposition is
computed for each DLL model. For instance:

./dll_decomposition --dll-model Conic myimage.png

will compute the decomposition of each objects boundaries into Conic DLL
segments. The result is a PNG image named Conic_out_myimage.png.

To compute two decompositions (one with the StraightLine DLL model and one with
the Circle DLL model) you can use the following command:

./dll_decomposition -d StraightLine -d Circle myimage.png

The result is two images named StraightLine_out_myimage.png and
Conic_out_myimage.png.

A --verbose switch (-v) is also available to print the DLL inequations on
the terminal standard output as well as the coordinates of the points composing
the DLL segments.

Images with a white background should be used as input images, but a
--black-background (-b) command line switch is available (and should be used)
to specify that the input image has a black background.
