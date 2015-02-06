// Copyright 2012-2014 Nicolas Normand <Nicolas.Normand@polytech.univ-nantes.fr>
// 
// This file is part of LUTBasedNSDistanceTransform.
// 
// LUTBasedNSDistanceTransform is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
// 
// LUTBasedNSDistanceTransform is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with
// LUTBasedNSDistanceTransform.  If not, see <http://www.gnu.org/licenses/>.
//
// $Id: LUTBasedNSDistanceTransform.cpp 162 2014-03-27 17:40:28Z Nicolas.Normand $

/**
 * @file LUTBasedNSDistanceTransformConfig.h.in
 * @author Nicolas Normand <Nicolas.Normand@polytech.univ-nantes.fr>
 * IRCCyN UMR 6597/Polytech Nantes
 *
 * This file is part of LUTBasedNSDistanceTransform.
 */

/**
 * @mainpage A Streaming Distance Transform Algorithm for Neighborhood-Sequence Distances
 *
 * @section presentation Presentation
 *
 * The purpose of LUTBasedNSDistanceTransform is to compute a "translated" 2D
 * Neighborhood-Sequence Distance Transform (DT) using a look up table
 * approach. It requires a single raster scan of the input image and produces
 * one line of output for every line of input. The neighborhood sequence is
 * specified either by providing one period of a periodic sequence either by
 * providing the rate of appearance of neighborhoods. The simple distances
 * @f$d_4@f$ and @f$d_8@f$. A second algorithm optionally derives the regular
 * (centered) DT from the "translated" DT, providing the result image
 * on-the-ﬂy, with a minimal delay, before the input image is fully processed.
 *
 * Classes D4Distance, D8Distance, PeriodicNSDistance and RatioNSDistance
 * are concrete factories for the classes D4DistanceTransform,
 * D8DistanceTransform, PeriodicNSDistanceTransform and
 * RatioNSDistanceTransform that compute translated distance transforms and
 * for D4DistanceTransformUntranslator, D8DistanceTransformUntranslator,
 * PeriodicNSDistanceTransformUntranslator and
 * RatioNSDistanceTransformUntranslator that recenter those transforms.
 *
 * This method is published in <a href="http://www.ipol.im/">Image Processing On Line</a>,
 * "A Streaming Distance Transform Algorithm for Neighborhood-Sequence
 * Distances", by Nicolas Normand, Robin Strand, Pierre Evenou and Aurore
 * Arlicot, doi: <a href="http://dx.doi.org/10.5201/ipol.2014.68">10.5201/ipol.2014.68</a>.
 */

#include <assert.h>
#include <limits.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>

#include "ImageReader.h"

#include "BaseDistanceDT.h"
#include "D4DistanceDT.h"
#include "D8DistanceDT.h"
#include "RatioNSDistanceDT.h"
#include "PeriodicNSDistanceDT.h"

#include "ImageWriter.h"

void usage() __attribute__ ((noreturn));

int parseRate(char* string, int *num, int *den) {
    char *startptr = string;
    char *endptr;

    *num = strtol(startptr, &endptr, 0);
    if (startptr == endptr) {
	fprintf(stderr, "Invalid sequence \"%s\"\n", endptr);
	return 0;
    }
    // Swallow space characters
    while (isspace(endptr[0]))
	endptr++;
    if (*endptr != '/') {
	fprintf(stderr, "Invalid sequence \"%s\"\n", endptr);
	return 0;
    }
    startptr = endptr + 1;
    *den = strtol(startptr, &endptr, 0);
    if (startptr == endptr) {
	fprintf(stderr, "Invalid sequence \"%s\"\n", endptr);
	return 0;
    }

    return 1;
}

int parseSequence(char* string, int *period, int **points) {
    char *startptr = string;
    char *endptr;
    int count = 0;

    // 1st pass: dry run, count integers
    do {
	//printf("Now parsing \"%s\"\n", startptr);
	int x = strtol(startptr, &endptr, 0);
	// Swallow space characters
	while (isspace(endptr[0]))
	    endptr++;
	if (startptr == endptr || x < 1 || x > 2) {
	    //if (endptr[0] != '\0' && endptr[0] != ',' )
	    {
		fprintf(stderr, "Invalid sequence \"%s\"\n", endptr);
		usage();
	    }
	}
	//printf("%d [%d-%d[ ", x, startptr-string, endptr-string);
	if (endptr[0] == ',')
	    endptr++;
	startptr = endptr;
	count++;
    } while (startptr[0] != '\0');
    //printf("\n");

    assert(points > 0);

    *points = (int *)malloc(count * sizeof(int));
    count = 0;

    startptr = string;
    do {
	int x = strtol(startptr, &endptr, 0);
	// Swallow space characters
	while (isspace(endptr[0]))
	    endptr++;
	if (endptr[0] == ',')
	    endptr++;
	startptr = endptr;
	(*points)[count++] = x;
    } while (startptr[0] != '\0');

    *period = count;

    return 1;
}

void usage() {
    fprintf(stderr,
	    //-----------------------------------------------------------------------------//
	    "Usage: LUTBasedNSDistanceTransform [-f filename] [-c] (-4|-8|-r <num/den>|-s <sequence>) [-t (pgm|png)]\n"
	    "\n"
	    "LUTBasedNSDistanceTransform computes the 2D translated neighborhood-sequence\n"
	    "distance transform of a binary image. It reads the input images from its\n"
	    "standard input and writes the result to its standard output.\n"
	    "\n"
	    "Options\n"
	    "  -4            Use the city block distance.\n"
	    "  -8            Use the chessboard distance.\n"
	    "  -s sequence   One period of the sequence of neighborhoods given as a list of 1\n"
	    "                and 2 separated by \" \" or \",\". Space characters must be escaped\n"
	    "                from the shell.\n"
	    "  -r num/den    Ratio of neighborhood 2 given as the rational number num/den\n"
	    "                (with den >= num >= 0 and den > 0).\n"
	    "  -c            Center the distance transform (the default is an asymmetric\n"
	    "                distance transform).\n"
	    "  -m value      Set the maximal value of the distance.\n"
	    "  -f filename   Read from file \"filename\" instead of stdin.\n"
	    "  -l            Flush output after each produced row.\n"
	    "  -t format     Select output image format (pgm or png).\n"
	    "\n"
	    "Examples:\n"
	    "  Translated octagonal distance transform:\n"
	    "    \"./LUTBasedNSDistanceTransform -r 1/2 < image.pbm\" or\n"
	    "    \"./LUTBasedNSDistanceTransform -s '1 2' < image.pbm\"\n"
	    "  Regular octagonal distance transform:\n"
	    "    \"./LUTBasedNSDistanceTransform -r 1/2 -c < image.pbm\" or\n"
	    "    \"./LUTBasedNSDistanceTransform -s '1 2' -c < image.pbm\"\n"
	    "  d4 distance transform:\n"
	    "    \"./LUTBasedNSDistanceTransform -r 0/1 -c < image.pbm\" or\n"
	    "    \"./LUTBasedNSDistanceTransform -s '1' -c < image.pbm\"\n"
	    "  d8 distance transform:\n"
	    "    \"./LUTBasedNSDistanceTransform -r 1/1 -c < image.pbm\" or\n"
	    "    \"./LUTBasedNSDistanceTransform -s '2' -c < image.pbm\"\n"
	    //-----------------------------------------------------------------------------//
	    );
    exit(-1);
}

typedef enum {
    undefined,
    d4,
    d8,
    ratioDefined,
    sequenceDefined
} DistanceType;

#define PBM_FILE_FORMAT	1
#define PNG_FILE_FORMAT	2

int main(int argc, char** argv) {
    //int row;
    FILE *input = stdin;
    int inputFormat = 0;
    DistanceType type = undefined;
    char *spec = NULL;
    int translateFlag = 0;
    char *myName = argv[0];
    char *outputFormat = NULL;
    bool lineBuffered = false;
    int dMax = 0;
    BaseDistance *dist = NULL;

    int ch;

    optind = 1;
    while ((ch = getopt(argc, argv, "m:t:f:48r:s:cl")) != -1) {
	switch (ch) {
	    case '4':
		if (type != undefined) {
		    fprintf(stderr, "Distance specified more than once\n");
		    exit(-1);
		}
		type = d4;
		break;
	    case '8':
		if (type != undefined) {
		    fprintf(stderr, "Distance specified more than once\n");
		    exit(-1);
		}
		type = d8;
		break;
	    case 'r': 
		if (type != undefined) {
		    fprintf(stderr, "Distance specified more than once\n");
		    exit(-1);
		}
		type = ratioDefined;
		spec = optarg;
		break;
	    case 's': 
		if (type != undefined) {
		    fprintf(stderr, "Distance specified more than once\n");
		    exit(-1);
		}
		type = sequenceDefined;
		spec = optarg;
		break;
	    case 'l':
		lineBuffered = true;
		break;
	    case 'm':
		char *endPtr;
		dMax = strtol(optarg, &endPtr, 10);
		if (*endPtr != '\0') {
		    fprintf(stderr, "Unable to parse maximal value \"%s\"\n", optarg);
		    exit(-1);
		}
		if (dMax < 0) {
		    fprintf(stderr, "The maximal distance value cannot be negative.\n");
		    exit(-1);
		}
		if (dMax > GRAYSCALE_MAX) {
		    fprintf(stderr, "The maximal distance value cannot exceed %d.\n", GRAYSCALE_MAX);
		    exit(-1);
		}
		break;
	    case 't':
		outputFormat = optarg;
		break;
	    case 'c':
		translateFlag = 1;
		break;
	    case 'f':
		if ((input = fopen(optarg, "r")) == NULL) {
		    fprintf(stderr, "%s (%s line %d): %s: %s\n", myName, __FILE__, __LINE__, optarg, strerror(errno));
		    exit(-1);
		}
		break;
 	    default:
		usage();
	}
    }
    argc -= optind;
    argv += optind;
    //optreset = 1;

    assert(input);
    if (argc != 0)
	usage();

    if (type == undefined) {
	fprintf(stderr, "Distance not specified\n");
	exit(-1);
    }

    switch (type) {
	case d4:
	    dist = new D4Distance(dMax);
	    break;
	case d8:
	    dist = new D8Distance(dMax);
	    break;
	case ratioDefined: {
	    int num, den;
	    if (!parseRate(spec, &num, &den)) {
		fprintf(stderr, "Unable to parse num/den \"%s\"\n", argv[0]);
		exit(-1);
	    }
	    if (num < 0 || den < num || den <= 0) {
		fprintf(stderr,
			"Invalid ratio %d/%d\n"
			"correct ratios num/den are such that den >= num >= 0 and den > 0\n",
			num, den);
		exit(-1);
	    }
	    if (num == 0) {
		dist = new D4Distance(dMax);
	    }
	    else if (num == den) {
		dist = new D8Distance(dMax);
	    }
	    else {
		dist = new RatioNSDistance(num, den, dMax);
	    }
	}
	    break;
	case sequenceDefined: {
	    int period = 0; int *sequence = NULL;

	    if (!parseSequence(spec, &period, &sequence)) {
		fprintf(stderr, "Unable to parse sequence \"%s\"\n", argv[0]);
		exit(-1);
	    }
	    int countOfNeighbors[2] = {0, 0};
	    int i;
	    for (i = 0; i < period; i++) {
		countOfNeighbors[sequence[i] - 1]++;
	    }
	    if (countOfNeighbors[0] == 0) {
		// d8
		dist = new D8Distance(dMax);
	    }
	    else if (countOfNeighbors[1] == 0) {
		// d4
		dist = new D4Distance(dMax);
	    }
	    else {
		dist = new PeriodicNSDistance(period, sequence, dMax);
	    }		
	    free(sequence);
	    sequence = NULL;
	}
	    break;
    }

    ImageConsumer<GrayscalePixelType> *output = createImageWriter("-", outputFormat, lineBuffered);
    if (output == NULL) {
	fprintf(stderr, "Unable to create image output stream (wrong format?)\n");
    }

    //pm_init(myName, 0);

    // TODO: check that the image consumers are deleted
    if (translateFlag) {
	output = dist->newDistanceTransformUntranslator(output);
    }
    BaseDistanceTransform *dt = dist->newTranslatedDistanceTransform(output);

#ifdef WITH_NETPBM
    if (inputFormat == 0) {
	char c = fgetc(input);
	ungetc(c, input);

	if (c == 'P') {
	    inputFormat = PBM_FILE_FORMAT;
	}
    }

    if (inputFormat == PBM_FILE_FORMAT) {
	PBMImageReader producer(dt, input);
	dt = NULL;

	while (!feof(input)) {
	    producer.produceAllRows();

	    int c;
	    while ((c = fgetc(input)) == '\n');
	    if (c != EOF)
		ungetc(c, input);
	}
    }
#endif

#ifdef WITH_PNG
    unsigned char signature[8];
    int readBytes = 0;

    if (inputFormat == 0) {
	readBytes = fread(signature, 1, 8, input);
    }
    
    if (inputFormat == PNG_FILE_FORMAT ||
	inputFormat == 0 && readBytes == 8 && png_check_sig(signature, 8)) {

	inputFormat = PNG_FILE_FORMAT;
	PNGImageReader producer(dt, input);
	dt = NULL;

	do {
	    // Assumes following images, if any, are png
	    producer.produceAllRows(8);
	}
	while (fread(signature, 1, 8, input) == 8);
    }
#endif

    if (inputFormat == 0) {
	fprintf(stderr, "Input image format not recognized\n");
    }

    if (dt != NULL) {
	delete dt;
    }

    fclose(input);
    return 0;
}
