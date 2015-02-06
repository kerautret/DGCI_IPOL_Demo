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
// $Id: ImageFilter.h 158 2014-03-25 15:58:51Z Nicolas.Normand $

/**
 * @file ImageFilter.h
 * @author Nicolas Normand <Nicolas.Normand@polytech.univ-nantes.fr>
 * IRCCyN UMR 6597/Polytech Nantes
 *
 * Interface to ImageFilter class
 *
 * An image filter receives images one row at a time, processes it, and forwards
 * the result to the following filter (or image consumer).
 * 
 * This file is part of LUTBasedNSDistanceTransform.
 */


#ifndef IMAGE_FILTER_H
#define IMAGE_FILTER_H

#include <sys/types.h>

#include <assert.h>
#include <algorithm>
#include <iostream>

#include "LUTBasedNSDistanceTransformConfig.h"

void freerow(void *);

/**
 * An ImageConsumer accepts pixels one row at a time in the standard scan
 * order.  A call to beginOfImage() prepares the ImageConsumer to receive an
 * image, it is followed by as many calls to processRow() as the number of
 * rows in the image and a final call to endOfImage().
 */
template <typename inputPixelType>
class ImageConsumer {
public:
    /**
     * Called by a producer to provide a row of pixels to this image
     * consumer.
     * 
     * Before processRow() is called, the image consumer must be inited
     * first using beginOfImage().
     *
     * @param inputRow the row of input pixels.
     */
    virtual void processRow(const inputPixelType* inputRow) = 0;
    /**
     * Prepare the consumer for an image.
     *
     * The consumer will allocate whatever structures necessary for the
     * processing of a new image.
     *
     * Calling beginOfImage() is mandatory before starting sending pixels
     * with processRow().
     *
     * @param cols the image width.
     * @param rows the image height.
     */
    virtual void beginOfImage(int cols, int rows) = 0;
    /**
     * End the processing of an image.
     *
     * Called by the producer when an image is fully transmitted. The
     * consumer may release structures allocated for the processing of this
     * image.
     *
     * endOfImage() must be called after the last row is processed through
     * processRow(). After endOfImage() is called, beginOfImage() may be
     * called again to start the processing of a new image.
     */
    virtual void endOfImage() = 0;
};

/**
 * A RowImageProducer generates image data row by row. The data is pushed to
 * the ImageConsumer attached to it.
 */
template <typename outputPixelType>
class RowImageProducer {
public:
    RowImageProducer(ImageConsumer<outputPixelType>* consumer) : _consumer(consumer) {
	if (_consumer == NULL) {
	    exit(1);
	}
    }
    /**
     * Tell if the producer can produce more rows of image.
     */
    bool hasMoreRows();
    /**
     * @brief Produce one row of an image.
     *
     * Ask the producer to produce one row of image. The producer will
     * provide the pixels to its consumer via ImageConsumer::processRow().
     */
    void produceRow();
    /**
     * @brief Produce all rows of an image.
     *
     * Ask the producer to produce all the image rows. The producer will
     * provide the pixels to its consumer via ImageConsumer::processRow().
     */
    void produceAllRows();
    virtual ~RowImageProducer() { delete _consumer; }
protected:
    ImageConsumer<outputPixelType>* _consumer;
};

/**
 * An ImageFilter is both an image consumer and a producer.
 * It redirects calls to beginOfImage(), processRow() and endOfImage() to the
 * next ImageConsumer (optionally modifying the size and content of the image).
 */
template <typename inputPixelType, typename outputPixelType>
class ImageFilter: public ImageConsumer<inputPixelType> {
public:
    ImageFilter(ImageConsumer<outputPixelType>* consumer) : _consumer(consumer) {
	if (_consumer == NULL) {
	    exit(1);
	}
    }
    virtual ~ImageFilter() { delete _consumer; }
    /**
     * Default ImageConsumer::beginOfImage() implementation.
     *
     * Default implementation of ImageConsumer::beginOfImage(): tell the
     * next image consumer in the chain to prepare for a new image.
     */
    void beginOfImage(int cols, int rows) {_consumer->beginOfImage(cols, rows);}
    /**
     * Default ImageConsumer::endOfImage() implementation.
     *
     * Default implementation of ImageConsumer::endOfImage(): tell the next
     * image consumer to end the image production.
     */
    void endOfImage() {_consumer->endOfImage();}
protected:
    /** Next ImageConsumer in the filter chain. */
    ImageConsumer<outputPixelType>* _consumer;
};

/**
 * A TeeImageFilter forwards the image data to two ImageConsumer intances
 * instead of one.
 */
template <typename inputPixelType, typename outputPixelType>
class TeeImageFilter: public ImageFilter<inputPixelType, outputPixelType> {
public:
    typedef ImageFilter<inputPixelType, outputPixelType> super;

    TeeImageFilter(ImageConsumer<inputPixelType>* consumer, ImageConsumer<inputPixelType>* consumer2) :
	ImageFilter<inputPixelType, outputPixelType>(consumer),
	_consumer2(consumer2) {}
    ~TeeImageFilter() { delete _consumer2; }

    /**
     * Start the processing of an image.
     *
     * TeeImageFilter::beginOfImage() invokes beginOfImage() on both its
     * consumers.
     */
    void beginOfImage(int cols, int rows) {
	super::beginOfImage(cols, rows);

	if (_consumer2) {
	    _consumer2->beginOfImage(cols, rows);
	}
    }

    void processRow(const outputPixelType* inputRow);
    /*
    void processRow(const outputPixelType* inputRow) {
	_consumer->processRow(inputRow);

	if (_consumer2) {
	    _consumer2->processRow(inputRow);
	}
    }*/

    /**
     * End the processing of an image.
     *
     * TeeImageFilter::endOfImage() invokes endOfImage() on both its
     * consumers.
     */
    void endOfImage() {
	super::endOfImage();

	if (_consumer2) {
	    _consumer2->endOfImage();
	}
    }
protected:
    ImageConsumer<outputPixelType>* _consumer2;
};

#endif
