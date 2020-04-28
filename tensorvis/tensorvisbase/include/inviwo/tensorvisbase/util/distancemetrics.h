/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2020 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#pragma once

#include <vector>
#include <inviwo/core/util/enumtraits.h>
#include <inviwo/core/util/assertion.h>
#include <inviwo/core/properties/optionproperty.h>

namespace inviwo {
namespace util {
enum class DistanceMetric { Euclidean, Manhattan, Minkowski, SquaredSum };

/**
 * \brief Calculates Minkowski distance of order n
 *        Input: Two vectors, each of which represent one point in an n-dimensional space
 *        Output: Minkowski distance of order n between the two points
 *
 * \param a Point a
 * \param b Point b
 * \param order Order of the Minkowski distance
 * \return Minkowski distance of order n between points a and b
 */
template <typename T>
T minkowskiDistance(const std::vector<T>& a, const std::vector<T>& b, const T order) {
    IVW_ASSERT(a.size() == b.size(), "Minkowski distance requires equal length vectors");
    return std::pow(std::inner_product(a.begin(), a.end(), b.begin(), T(0), std::plus<>(),
                                       [order](T x, T y) { return std::pow(y - x, order); }),
                    T(1) / order);
}

/**
 * \brief Calculates Manhattan distance
 *        Input: Two vectors, each of which represent one point in an n-dimensional space
 *        Output: Manhattan distance between the two points
 *
 * \param a Point a
 * \param b Point b
 * \return Manhattan distance between points a and b
 */
template <typename T>
T manhattanDistance(const std::vector<T>& a, const std::vector<T>& b) {
    return minkowskiDistance(a, b, T(1));
}

/**
 * \brief Calculates Euclidean distance
 *        Input: Two vectors, each of which represent one point in an n-dimensional space
 *        Output: Euclidean distance between the two points
 *
 * \param a Point a
 * \param b Point b
 * \return Euclidean distance between points a and b
 */
template <typename T>
T euclideanDistance(const std::vector<T>& a, const std::vector<T>& b) {
    return minkowskiDistance(a, b, T(2));
}

/**
 * \brief Calculates squared sum distance
 *        Input: Two vectors, each of which represent one point in an n-dimensional space
 *        Output: Squared sum distance between the two points
 *
 * \param a Point a
 * \param b Point b
 * \return Squared sum distance between points a and b
 */
template <typename T>
T squaredSumDistance(const std::vector<T>& a, const std::vector<T>& b) {
    IVW_ASSERT(a.size() == b.size(), "Squared sum distance requires equal length vectors");
    return std::inner_product(a.begin(), a.end(), b.begin(), T(0), std::plus<>(),
                              [](T x, T y) { return std::pow(y - x, T(2)); });
}

template <typename T>
inline std::function<T(const std::vector<T>&, const std::vector<T>&)> getDistanceMetricFunctor(
    DistanceMetric metric) {

    switch (metric) {
        case DistanceMetric::Euclidean:
            return euclideanDistance<T>;
        case DistanceMetric::Manhattan:
            return manhattanDistance<T>;
        case DistanceMetric::SquaredSum:
            return squaredSumDistance<T>;
        default:
            break;
    }

    return [](const std::vector<T>&, const std::vector<T>&) { return T(0); };
}

}  // namespace util
template <>
struct EnumTraits<util::DistanceMetric> {
    static std::string name() { return "DistanceMetric"; }
};

}  // namespace inviwo