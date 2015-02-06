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
// $Id: RatioNSDistanceDT.h 161 2014-03-27 16:32:50Z Nicolas.Normand $

/**
 * @file RatioNSDistanceDT.h
 * @author Nicolas Normand <Nicolas.Normand@polytech.univ-nantes.fr>
 * IRCCyN UMR 6597/Polytech Nantes
 *
 * @brief This file gathers classes related to neighborhood-sequence
 * distances defined by a ratio of neighborhoods.
 * 
 * This file is part of LUTBasedNSDistanceTransform.
 */

#include "BaseDistanceDT.h"
#include "RationalBeattySequence.h"

/**
 * @brief ratio-based neighborhood-sequence distance.
 *
 * RatioNSDistance is a concrete factory for filters related to
 * neighborhood-sequence distances defined by a ratio of neighborhoods:
 * RatioNSDistanceTransform and RatioNSDistanceTransformUntranslator.
 */
class RatioNSDistance: public BaseDistance {
public:
    /**
     * @param num numerator
     * @param den denominator. The ratio \p num/\p den represents the ratio
     * of occurrences of the value 2 in the sequence.
     * @param dMax maximal value of the distance transform. This value is
     * passed to the RatioNSDistanceTransform and
     * RatioNSDistanceTransformUntranslator constructors.
     *
     * The neighborhood-sequence distance between two points is the minimal
     * length of a path that joins the points. The kind of neighborhood that
     * can be used at a given step in a path is specified by the sequence.
     * For instance, the octagonal distance uses alternatively neighborhoods
     * 1 (horizontal and vertical displacements only, also known as \em
     * 4-neighborhood) and 2 (same with diagonal displacements, also known
     * as \em 8-neighborhood). The octagonal distance can be constructed
     * with \p num = 1 and \p den = 2.
     */
    RatioNSDistance(int num, int den, GrayscalePixelType dMax = 0);
    /**
     * @brief create a distance transform ImageFilter for a translated
     * neighborhood-sequence distance.
     * @return an instance of RatioNSDistanceTransform.
     */
    BaseDistanceTransform* newTranslatedDistanceTransform(ImageConsumer<GrayscalePixelType>* consumer) const;
    /**
     * @brief create an ImageFilter to recenter the translated distance
     * transform of a neighborhood-sequence distance.
     * @return an instance of RatioNSDistanceTransformUntranslator.
     */
    DistanceTransformUntranslator<GrayscalePixelType, GrayscalePixelType>* newDistanceTransformUntranslator(ImageConsumer<GrayscalePixelType>* consumer) const;

protected:
    GrayscalePixelType _dMax;
    int num;
    int den;
    /**
     * A RationalBeattySeq that represents the numbers of occurrences of
     * 1 from the beginning of the sequence of neighborhoods.
     */
    RationalBeattySeq mbf1;
    /**
     * A RationalBeattySeq that represents the numbers of occurrences of
     * 2 from the beginning of the sequence of neighborhoods.
     */
    RationalBeattySeq mbf2;
    /**
     * Inverse of #mbf1. #mbf1 and #mbf1i are used to predict the next
     * occurrence of a 1 after a given index.
     */
    RationalBeattySeq mbf1i;
    /**
     * Inverse of #mbf2. #mbf2 and #mbf2i are used to predict the next
     * occurrence of a 2 after a given index.
     */
    RationalBeattySeq mbf2i;

  friend class RatioNSDistanceTransform;    
  friend class RatioNSDistanceTransformUntranslator;    
};

/**
 * @brief Periodic neighborhood-sequence distance transform.
 *
 * RatioNSDistanceTransform is an ImageFilter that computes the translated
 * distance transform of a ratio-based neighborhood-sequence distance. The
 * input image is provided one row at a time by processRow().  The result image
 * is written in the next consumer in the filter chain.
 */
class RatioNSDistanceTransform : public BaseDistanceTransform {
public:
    /**
     * @brief Construct a RatioNSDistanceTransform
     *
     * @param num numerator
     * @param den denominator. The ratio \p num/\p den represents the ratio
     * of occurrences of the value 2 in the sequence.
     * @param dMax maximal value of the distance transform. Output distance
     * values are saturated to this value. When \p dMax is 0, the distance
     * is bounded by #GRAYSCALE_MAX.
     */
    RatioNSDistanceTransform(ImageConsumer<GrayscalePixelType>* consumer, int num, int den, GrayscalePixelType dMax = 0);

    /**
     * @brief Process one row of image.
     *
     * Process one row of a binary image and write the translated distance
     * transform result to the next consumer in the filter chain.
     *
     * @param imageRow the image row to process.
     *
     * For each input pixel @f$p@f$, processRow computes @f$DT_X(p)@f$:
     * @f{equation}{
     *   DT_X(p)=\begin{cases}
     *     0&\text{if }p\not\in X\\
     *     \min\big\{\hat C_{\vec v}(DT_X(p-\vec v)),\vec v\in\mathcal N'\big\}&\text{otherwise}
     *   \end{cases}
     * @f}
     * where @f$\mathcal N'@f$ is the translated neighborhood and the
     * displacement costs are:
     * @f{equation}{
     *   \hat C_{\vec v}(r)=\begin{cases}
     *     \hat C_{\vec v}^1(r)   =\mathbf1_B^\dagger(\mathbf1_B(r) + 1) + 1&\text{if }\vec v\in\mathcal{N}'_1\text{ and }\vec v\not\in\mathcal{N}'_2\\
     *     \hat C_{\vec v}^2(r)   =\mathbf2_B^\dagger(\mathbf2_B(r) + 1) + 1&\text{if }\vec v\not\in\mathcal{N}'_1\text{ and }\vec v\in\mathcal{N}'_2\\
     *     \hat C_{\vec v}^{12}(r)=r + 1                                    &\text{if }\vec v\in\mathcal{N}'_1\text{ and }\vec v\in\mathcal{N}'_2\\
     *     \infty                                                           &\text{otherwise}
     *   \end{cases}\;.
     * @f}
     * The values of @f$\mathbf1_B(r)@f$, @f$\mathbf2_B(r)@f$,
     * @f$\mathbf1_B^\dagger(r)@f$ and @f$\mathbf2_B^\dagger(r)@f$ are provided
     * by methods RatioNSDistance::mbf1, RatioNSDistance::mbf2,
     * RatioNSDistance::mbf1i and RatioNSDistance::mbf2i.
     * 
     * Points @f$p-v@f$ belong to the two previous lines of the transform. The class
     * BaseDistanceTransform keeps those lines in a circular buffer.
     *
     * The distance transform values are clamped to the upper bound #_dMax.
     *
     */
    void processRow(const BinaryPixelType *imageRow);

protected:
    /**
     * Upper bound of the distance transform value (output distance values
     * are saturated to this value).
     */
    GrayscalePixelType _dMax;
    /** Distance */
    const RatioNSDistance d;
};

/**
 * @brief Ratio-based neighborhood-sequence distance transform untranslator.
 *
 * RatioNSDistanceTransform is an ImageFilter that recenters the translated
 * distance transform of a ratio-based neighborhood-sequence distance. The
 * input image is provided one row at a time by processRow().  The result
 * image is written in the next consumer in the filter chain.
 */
class RatioNSDistanceTransformUntranslator: public DistanceTransformUntranslator<GrayscalePixelType, GrayscalePixelType> {
public:
    /**
     * @brief Construct a RatioNSDistanceTransformUntranslator.
     *
     * @param consumer the next consumer in the filter chain.
     * @param num numerator
     * @param den denominator. The ratio \p num/\p den represents the ratio
     * of occurrences of the value 2 in the sequence.
     * @param dMax maximal value of the distance transform. Input distance
     * values are assumed never to exceed this value. When \p dMax is 0, the
     * distance is only bounded by #GRAYSCALE_MAX.
     *
     * The algorithm latency (and the number of rows that have to be stored)
     * directly depends on the maximal value of the distance transform.
     * Forcing a lower value sets an upper bound on the latency and reduces
     * the allocated memory.
     */
    RatioNSDistanceTransformUntranslator(ImageConsumer<GrayscalePixelType>* consumer, int num, int den, GrayscalePixelType dMax = 0);
    ~RatioNSDistanceTransformUntranslator();

    void beginOfImage(int cols, int rows);
    void processRow(const GrayscalePixelType* inputRow);

protected:
    /**
     * Upper bound of the distance transform value (input distance values
     * are assumed never to exceed #_dMax).
     */
    GrayscalePixelType _dMax;
    /** Distance */
    const RatioNSDistance d;

private:
    static const int marginRight = 1;
    typedef DistanceTransformUntranslator<GrayscalePixelType, GrayscalePixelType> super;
};

