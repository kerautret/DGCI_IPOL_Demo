
This is the source code and tools corresponding to the "A Near-Linear Time Guaranteed Algorithm for Digital Curve Simplification Under the Fréchet Distance" version 1.1 IPOL article ( http://dx.doi.org/10.5201/ipol.2014.70 )

Author: Isabelle Sivignon

Copyright (c) 2014 by Isabelle Sivignon
License: GNU LESSER GENERAL PUBLIC LICENSE (the "LGPL").

Version 1.4 06/02/2015


The program by default doesn't need to import external libraries (the static boost lib is already in the archive)

--------------------------
Compilation instructions:
--------------------------
To compile the demonstration code given in (demoIPOL_FrechetSimplification), you just have to do:

cd FrechetAndConnectedCompDemo
mkdir build
cd build
cmake ..
make 
You can install the files if you want:
sudo make install

Then the binary file "FrechetSimplification" will be available in the "FrechetAndConnectedCompDemo/build/demoIPOL_FrechetSimplification".

------------
Basic Usage:
------------
* From the "FrechetAndConnectedCompDemo/build/demoIPOL_FrechetSimplification" directory you can test:

./frechetSimplification -error 4 -sdp ../../demoIPOL_FrechetSimplification/Data/Plants/Plant019.sdp

It will produces the resulting polygon given as vertex set (output.txt) and displayed in output.eps


* If you want directly apply your algorithm from an grayscale image, you can do it by using the pgm2freeman executable given here:
 ../demoIPOL_ExtrConnectedReg/pgm2freeman -min_size 100.0 -image inputNG.pgm -outputSDPAll > inputContour.txt

All these contours can then be processed:
 ./frechetSimplification -error 4 -sdp inputContour.txt -allContours

The -allContours is important since the input format is supposed to be one polygon per line.


credits and acknowledgments:
Image from data are given from LEMS Vision Group at Brown University, under Professor Ben Kimia (http://www.lems.brown.edu/~dmc)/ 



-----------
Changelog
----------

Version 1.4 06/02/2015:
        - fix compillation pb on MacOSX
        - update boost version to 1.57
        - fix some warnings

Version 1.3 07/05/2014:
        - Fix issue in pgm2freeman prog (bis)

 
Version 1.2 07/05/2014:
        - Improve display result for IPOL result presentation.




