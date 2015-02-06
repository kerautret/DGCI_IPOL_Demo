This is the source code and tools corresponding to the "A Streaming Distance
Transform Algorithm for Neighborhood-Sequence Distances" version 1.1 IPOL
article ( http://dx.doi.org/10.5201/ipol.2014.68 )

LUTBasedNSDistanceTransform: Implements the algorithm of the Streaming
Distance Transform for Neighborhood-Sequence Distances.  Authors: Nicolas
Normand, Robin Strand, Pierre Evenou, Aurore Arlicot

Contacts: Nicolas.Normand@polytech.univ-nantes.fr
Pierre.Evenou@polytech.univ-nantes.fr Aurore.Arlicot@polytech.univ-nantes.fr
robin@cb.uu.se

Copyright (c) 2014 by Nicolas Normand <Nicolas.Normand@polytech.univ-nantes.fr>
This program is distributed under the terms of the GNU General Public
License Version 3 (GPLv3).

Version 1.1 19/02/2014

The program uses netpbm or libpng for image I/O. Both libraries provide
functions able to read images one row at a time.  To compile simply use
cmake (see INSTALL file)

------
Program help:
￼LUTBasedNSDistanceTransform [-f filename] [-c] [-t (pgm|png)] \ (-4|-8|-r
 <num/den>|-s <sequence>)
                     -4           Use the city block distance.
                     -8           Use the chessboard distance.
                     -s           sequence  One period of the sequence of
		                  neighborhoods given as a list of 1 and  2
				  separated by " " or ",". Space characters must
				  be escaped from the  shell.
                     -r num/den   Ratio of neighborhood 2 given as the rational
		                  number num/den (with den >= num >= 0 and
				  den > 0).
                     -c           Center the distance transform (the default is
		                  an asymmetric distance transform).
                     -f filename  Read from file "filename" instead of stdin.
                     -l           Flush output after each produced row.
                     -t format    Select output image format (pgm or png).

Example of usage for regular octagonal distance transform:
    ./LUTBasedNSDistanceTransform -r 1/2 -c < image.pbm 
 or
    ./LUTBasedNSDistanceTransform -s ’1 2’ -c < image.pbm

-------
Change from 1.0: adding FindPGM.cmake file.
All sources file were reviewed in the IPOL publication 
# credits and acknowledgments
# list of known defect:

