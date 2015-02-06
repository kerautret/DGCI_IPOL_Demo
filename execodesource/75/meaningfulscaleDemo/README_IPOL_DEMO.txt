
This is the source code and tools corresponding to the "Meaningful Scales Detection: an Unsupervised Noise Detection Algorithm for Digital Contours" IPOL article ( http://dx.doi.org/10.5201/ipol.2014.75 )

Authors: Bertrand Kerautret and Jacques-Olivier Lachaud

Copyright (c) 2014 by  Bertrand Kerautret, Jacques-Olivier Lachaud
License: GNU LESSER GENERAL PUBLIC LICENSE (the "LGPL").

Version 1.2 17/03/2014

The program by default doesn't need to import external libraries.

--------------------------
Compilation instructions:
--------------------------
To compile the demonstration code, you just have to do:

cd meaningfulscaleDemo
mkdir build
cd build
cmake ..
make
 
You can install the files if you want:
sudo make install

Then the binary file "meaningfulScaleEstim", will be available in the "meaningfulscaleDemo/build/demoIPOL/meaningfulScaleEstim".
Complementary binary to extract image contours (pgm2freeman)  will be available here: "meaningfulscaleDemo/build/bin"


------------------------------------------------
Basic Usage:  Noise detection on digital contour
------------------------------------------------

-----
From an existing digital contour (in freeman code format)
-----

* From the "meaningfulscaleDemo/build/demoIPOL" directory you can test by just extracting the noise level in an output file:
----
./meaningfulScaleEstim < ../../demoIPOL/Contours/ellipseBruit.fc -printNoiseLevel >  noiseLevel.txt
----

* You can also apply the noise level visualisation with the source contour: 
./meaningfulScaleEstim < ../../demoIPOL/Contours/ellipseBruit.fc -drawXFIGNoiseLevel -enteteXFIG -drawContourSRC 4 1 > tmp.fig
It generates an xfig figure that you can convert to eps file:
fig2dev -L eps tmp.fig tmp.eps




-----------
Changelog
----------

Version 1.3 17/04/2014:
        - Addition of minimalCodeNoiseDetection.cxx in demoIPOL
        - Fixing compilation issue in particular header use:
           -> fixing headers of file include/ImaGene/helper/MultiscaleProfile.h
        - Addition of the -version to be used in the demonstration archive.

Version 1.2 17/03/2014 (initial online version)
        


---------------
For more details see IPOL Journal article available here:  
 http://dx.doi.org/10.5201/ipol.2014.75




