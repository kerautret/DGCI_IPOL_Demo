
This is the source code and tools corresponding to the "Extraction of Connected Region Boundary in Multidimensional Images" version 1.2 IPOL article ( http://dx.doi.org/10.5201/ipol.2014.74 )

Authors: David Coeurjolly, Bertrand Kerautret, Jacques-Olivier Lachaud

Copyright (c) 2014 by  David Coeurjolly, Bertrand Kerautret, Jacques-Olivier Lachaud
License: GNU LESSER GENERAL PUBLIC LICENSE (the "LGPL").

Version 1.4 06/02/2015

The program by default doesn't need to import external libraries (the static boost lib is already in the archive)

--------------------------
Compilation instructions:
--------------------------
To compile the demonstration code given in (demoIPOL_ExtrConnectedReg), you just have to do:

cd FrechetAndConnectedCompDemo
mkdir build
cd build
cmake ..
make 
You can install the files if you want:
sudo make install

Then the binary file "pgm2freeman", "extract3D"  and "displayContours" will be available in the "FrechetAndConnectedCompDemo/build/demoIPOL_ExtrConnectedReg".



------------------------------------------------
Basic Usage:  2D connected component extraction
------------------------------------------------

* From the "FrechetAndConnectedCompDemo/build/demoIPOL_ExtrConnectedReg" directory you can test:

./pgm2freeman -image ../../examples/samples/contourS.pgm -badj 0 > freemanChainContours.fc

(by default the threshold will automatically set by the Otsu algorithm)

It will produces digital contour which can be visualized  by using the "displayContours" program:
./displayContours -fc freemanChainContours.fc -outputFIG imageContours.fig -backgroundImageXFIG ../../examples/samples/contourS.png 185 85

Then you can visualize it by using xfig or convert it using the fig2dev tool:
fig2dev -L eps imageContours.fig resu.eps 





------------------------------------------------
Basic Usage:  3D connected component extraction
------------------------------------------------

* From the "FrechetAndConnectedCompDemo/build/demoIPOL_ExtrConnectedReg" directory you can test:

./extract3D  -image ../../examples/samples/lobster.vol -threshold 190 255 -badj 0 


It will produces the 3d files "output.off" and "output.off.mtl"  representing each 3D connected region. 
These files can be displayed using for instance meshlab.



---------------
For more details see IPOL Journal article available here:  
 http://dx.doi.org/10.5201/ipol.2014.74





-----------
Changelog
----------

Version 1.4 06/02/2015:
        - fix compillation pb on MacOSX
        - update boost version to 1.57
        - fix some warnings

Version 1.3 07/05/2014:
        - Fix issue in pgm2freeman prog (bis)
        

Version 1.2 18/04/2014:
        - Fix issue in pgm2freeman prog
        - Addition of the -version to be used in the demonstration archive.

Version 1.1  (initial online version)






