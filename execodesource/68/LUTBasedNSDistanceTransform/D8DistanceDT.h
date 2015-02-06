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
// $Id: D8DistanceDT.h 161 2014-03-27 16:32:50Z Nicolas.Normand $

/**
 * @file D8DistanceDT.h
 * @author Nicolas Normand <Nicolas.Normand@polytech.univ-nantes.fr>
 * IRCCyN UMR 6597/Polytech Nantes
 *
 * @brief This file gathers classes related to the chessboard distance,
 * @f$d_8@f$.
 * 
 * This file is part of LUTBasedNSDistanceTransform.
 */

#include "BaseDistanceDT.h"

/**
 * @brief chessboard distance, @f$d_8@f$.
 *
 * D8Distance is a concrete factory for filters related to the chessboard
 * distance: D8DistanceTransform and D8DistanceTransformUntranslator.
 */
class D8Distance: public BaseDistance {
public:
    /**
     * @param dMax maximal value of the distance transform. This value is
     * passed to the D8DistanceTransform and D8DistanceTransformUntranslator
     * constructors.
     */
    D8Distance(GrayscalePixelType dMax = 0);
    /**
     * @brief create a distance transform ImageFilter for the translated
     * @f$d_8@f$ distance.
     * @return an instance of D4DistanceTransform.
     */
    BaseDistanceTransform* newTranslatedDistanceTransform(ImageConsumer<GrayscalePixelType>* consumer) const;
    /**
     * @brief create an ImageFilter to recenter translated @f$d_4@f$
     * distance transforms.
     * @return an instance of D4DistanceTransformUntranslator.
     */
    DistanceTransformUntranslator<GrayscalePixelType, GrayscalePixelType>* newDistanceTransformUntranslator(ImageConsumer<GrayscalePixelType>* consumer) const;

protected:
    const GrayscalePixelType _dMax;
};

/**
 * @brief @f$d_8@f$ distance transform.
 *
 * D8DistanceTransform is an ImageFilter that computes the translated
 * @f$d_8@f$ distance transform. The input image is provided one row at a
 * time by processRow(). The result image is written in the next consumer in
 * the filter chain.
 */
class D8DistanceTransform: public BaseDistanceTransform {
public:
    /**
     * @brief Construct a D8DistanceTransform
     *
     * @param consumer the next consumer in the filter chain.
     * @param dMax maximal value of the distance transform. Output distance
     * values are saturated to this value. When \p dMax is 0, the distance
     * is bounded by #GRAYSCALE_MAX.
     */
    D8DistanceTransform(ImageConsumer<GrayscalePixelType>* consumer, GrayscalePixelType dMax = 0);

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
     *     \min\big\{\hat C_{\vec v}(DT_X(p-\vec v)),\vec v\in\mathcal N'_8\big\}&\text{otherwise}
     *   \end{cases}
     * @f}
     * where @f$\mathcal N'_8*@f$ is the translated 8-neighborhood. Points @f$p-v@f$
     * belong to the two previous lines of the transform. The class
     * BaseDistanceTransform keeps those lines in a circular buffer.
     *
     * The distance transform values are clamped to the upper bound #_dMax.
     *
     * For distance @f$d_8@f$, @f$\hat C_{\vec v}@f$ is simply:
     * @f{equation}{
     *   \forall r\geq0,\hat C_{\vec v}(r)=r+1\;.
     * @f}
     */
    void processRow(const BinaryPixelType *imageRow);

protected:
    /**
     * Upper bound of the distance transform value (output distance values
     * are saturated to this value).
     */
    const GrayscalePixelType _dMax;
};

/** @brief @f$d_8@f$ distance transform untranslator.
 *
 * D4DistanceTransform is an ImageFilter that recenters the translated
 * @f$d_8@f$ distance transform. The input image is provided one row at a
 * time by processRow(). The result image is written in the next consumer in
 * the filter chain.
 */
class D8DistanceTransformUntranslator: public DistanceTransformUntranslator<GrayscalePixelType, GrayscalePixelType> {
public:
    /**
     * @brief Construct a D4DistanceTransformUntranslator.
     *
     * @param consumer the next consumer in the filter chain.
     * @param dMax maximal value of the distance transform. Input distance
     * values are assumed never to exceed this value. When \p dMax is 0, the
     * distance is only bounded by #GRAYSCALE_MAX.
     *
     * The algorithm latency (and the number of rows that have to be stored)
     * directly depends on the maximal value of the distance transform.
     * Forcing a lower value sets an upper bound on the latency and reduces
     * the allocated memory.
     */
    D8DistanceTransformUntranslator(ImageConsumer<GrayscalePixelType>* consumer, GrayscalePixelType dMax = 0);
    ~D8DistanceTransformUntranslator();

    void beginOfImage(int cols, int rows);
    void processRow(const GrayscalePixelType* inputRow);

protected:
    /**
     * Upper bound of the distance transform value (input distance values
     * are assumed never to exceed #_dMax).
     */
    const GrayscalePixelType _dMax;

private:
    typedef DistanceTransformUntranslator<GrayscalePixelType, GrayscalePixelType> super;
};

