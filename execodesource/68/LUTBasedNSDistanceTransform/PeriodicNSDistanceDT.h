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
// $Id: PeriodicNSDistanceDT.h 161 2014-03-27 16:32:50Z Nicolas.Normand $

/**
 * @file PeriodicNSDistanceDT.h
 * @author Nicolas Normand <Nicolas.Normand@polytech.univ-nantes.fr>
 * IRCCyN UMR 6597/Polytech Nantes
 *
 * @brief This file gathers classes related to neighborhood-sequence
 * distances defined by a periodic sequence.
 * 
 * This file is part of LUTBasedNSDistanceTransform.
 */

#include "BaseDistanceDT.h"

/**
 * @brief periodic neighborhood-sequence distance.
 *
 * PeriodicDistance is a concrete factory for filters related to
 * neighborhood-sequence distances defined by a periodic sequence:
 * PeriodicNSDistanceTransform and PeriodicNSDistanceTransformUntranslator.
 */
class PeriodicNSDistance: public BaseDistance {
public:
    /**
     * Construct a filter factory for a neighborhood-sequence distance
     * defined by a periodic sequence.
     *
     * @param period length of Bvalues.
     * @param Bvalues a period of the neighborhood sequence. Bvalues points
     * to a int array of values 1 and 2 of length \p period.
     * @param dMax maximal value of the distance transform. This value is
     * passed to the PeriodicNSDistanceTransform and
     * PeriodicNSDistanceTransformUntranslator constructors.
     *
     * The neighborhood-sequence distance between two points is the minimal
     * length of a path that joins the points. The kind of neighborhood that
     * can be used at a given step in a path is specified by the sequence.
     * For instance, the octagonal distance uses alternatively neighborhoods
     * 1 (horizontal and vertical displacements only, also known as \em
     * 4-neighborhood) and 2 (same with diagonal displacements, also known
     * as \em 8-neighborhood). The octagonal distance can be constructed
     * with \p period = 2 and \p Bvalues = {1,2}.
     */
    PeriodicNSDistance(int period, int *Bvalues, GrayscalePixelType dMax = 0);
    ~PeriodicNSDistance();
    /**
     * @brief create a distance transform ImageFilter for a translated
     * neighborhood-sequence distance.
     * @return an instance of PeriodicNSDistanceTransform.
     */
    BaseDistanceTransform* newTranslatedDistanceTransform(ImageConsumer<GrayscalePixelType>* consumer) const;
    /**
     * @brief create an ImageFilter to recenter the translated distance
     * transform of a neighborhood-sequence distance.
     * @return an instance of PeriodicNSDistanceTransformUntranslator.
     */
    DistanceTransformUntranslator<GrayscalePixelType, GrayscalePixelType>* newDistanceTransformUntranslator(ImageConsumer<GrayscalePixelType>* consumer) const;

    /**
     * @return number of occurrences of 2 in the sequence between indices 1
     * and \p r.
     */
    int countOf2(int r) const;
    /**
     * @return index of the next occurrence of 1 in the sequence. It is also
     * the absolute cost of displacement with a vector in translated
     * neighborhood 1 only (@f$\vec v\in\mathcal{N}'_1\text{ and }\vec
     * v\not\in\mathcal{N}'_2@f$).
     */
    GrayscalePixelType next1(int r) const;
    /**
     * @return index of the next occurrence of 2 in the sequence. It is also
     * the absolute cost of displacement with a vector in translated
     * neighborhood 2 only (@f$\vec v\not\in\mathcal{N}'_1\text{ and }\vec
     * v\in\mathcal{N}'_2@f$).
     */
    GrayscalePixelType next2(int r) const;
protected:
    const int period;
    const GrayscalePixelType _dMax;
    /**
     * Stores the relative costs of displacement with a vector in translated
     * neighborhood 1 only (@f$\vec v\in\mathcal{N}'_1\text{ and }\vec
     * v\not\in\mathcal{N}'_2@f$) for one period of the sequence.
     */
    int *c1;
    /**
     * Stores the relative costs of displacement with a vector in translated
     * neighborhood 2 only (@f$\vec v\not\in\mathcal{N}'_1\text{ and }\vec
     * v\in\mathcal{N}'_2@f$) for one period of the sequence.
     */
    int *c2;
    /**
     * Stores the number of occurences of 1 for each index in a period of
     * the sequence.
     */
    int *mathbf1d;
    /**
     * Stores the number of occurences of 2 for each index in a period of
     * the sequence.
     */
    int *mathbf2d;
};

/**
 * @brief Periodic neighborhood-sequence distance transform.
 *
 * PeriodicNSDistanceTransform is an ImageFilter that computes the
 * translated distance transform of a periodic neighborhood-sequence
 * distance. The input image is provided one row at a time by processRow().
 * The result image is written in the next consumer in the filter chain.
 */
class PeriodicNSDistanceTransform : public BaseDistanceTransform {
public:
    /**
     * @brief Construct a PeriodicNSDistanceTransform
     *
     * @param d a PeriodicNSDistance
     * @param dMax maximal value of the distance transform. Output distance
     * values are saturated to this value. When \p dMax is 0, the distance
     * is bounded by #GRAYSCALE_MAX.
     */
    PeriodicNSDistanceTransform(ImageConsumer<GrayscalePixelType>* consumer, const PeriodicNSDistance *d, GrayscalePixelType dMax = 0);
    ~PeriodicNSDistanceTransform();

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
     * Practically, the values of @f$\hat C_{\vec v}^1(r)@f$ and @f$\hat
     * C_{\vec v}^1(r)@f$ are provided by methods PeriodicNSDistance::next1
     * and PeriodicNSDistance::next2.
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
    const GrayscalePixelType _dMax;
    /** Distance */
    const PeriodicNSDistance *_d;
};

/** @brief Periodic neighborhood-sequence distance transform untranslator.
 *
 * PeriodicNSDistanceTransform is an ImageFilter that recenters the
 * translated distance transform of a periodic neighborhood-sequence
 * distance. The input image is provided one row at a time by processRow().
 * The result image is written in the next consumer in the filter chain.
 */
class PeriodicNSDistanceTransformUntranslator: public DistanceTransformUntranslator<GrayscalePixelType, GrayscalePixelType> {
public:
    /**
     * @brief Construct a PeriodicNSDistanceTransformUntranslator.
     *
     * @param consumer the next consumer in the filter chain.
     * @param d a periodic neighborhood-sequence distance.
     * @param dMax maximal value of the distance transform. Input distance
     * values are assumed never to exceed this value. When \p dMax is 0, the
     * distance is only bounded by #GRAYSCALE_MAX.
     *
     * The algorithm latency (and the number of rows that have to be stored)
     * directly depends on the maximal value of the distance transform.
     * Forcing a lower value sets an upper bound on the latency and reduces
     * the allocated memory.
     */
    PeriodicNSDistanceTransformUntranslator(ImageConsumer<GrayscalePixelType>* consumer, const PeriodicNSDistance *d, GrayscalePixelType dMax = 0);
    ~PeriodicNSDistanceTransformUntranslator();

    void beginOfImage(int cols, int rows);
    void processRow(const GrayscalePixelType* inputRow);

protected:
    /**
     * Upper bound of the distance transform value (input distance values
     * are assumed never to exceed #_dMax).
     */
    const GrayscalePixelType _dMax;
    /** Distance */
    const PeriodicNSDistance *_d;

private:
    static const int marginRight = 1;
    typedef DistanceTransformUntranslator<GrayscalePixelType, GrayscalePixelType> super;
};

