This program is written by Benoît Naegel <b.naegel@unistra.fr> and Nicolas Passat <passat@unistra.fr>
distributed under the terms of the GPLv3 license.

This program is a command-line (non interactive) implementation of the image segmentation method as described in:

    -https://edit.ipol.im/edit/algo/np_dgci_interactive_segmentation/

    -Nicolas Passat, Benoît Naegel, François Rousseau, Mériam Koob, Jean-Louis Dietemann.
     Interactive segmentation based on component-trees.
     Pattern Recognition, (44)10-11:2539-2554, 2011.

    -Nicolas Passat and Benoît Naegel.
     Selection of relevant nodes from component-trees in linear time.
     DGCI 2011, 16th International Conference on Discrete Geometry for Computer Imagery. Nancy, France, April 6-8, 2011.
     Lecture Notes in Computer Science, Vol. 6607, pp 453-464 (Springer).

This program is working only with PGM grey-level images
To convert a grey-level image from any "standard" format in PGM you can use ImageMagick:
    -convert image.png image.pgm
Command line: ctseg <source> <marker> <alpha> with:
 -<source> : name of an existing PGM 8 bits image
 -<marker> : name of an existing PGM 8 bits image
 -<alpha>  : floating number (0<= alpha <=1)
 -[negate] : negate the source image.
	    By default, the program assumes bright objects on dark background.
	    To extract dark objects on bright background, the source image must be negated
	    (which is equivalent to compute the dual component-tree (min-tree)
