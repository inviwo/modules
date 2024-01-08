/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2024 Inviwo Foundation
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

#ifndef IVW_SHANNONENTROPY2D_H
#define IVW_SHANNONENTROPY2D_H

#include <inviwo/integrallinefiltering/integrallinefilteringmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/util/imageramutils.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/datastructures/image/layer.h>
#include <inviwo/core/datastructures/image/layerram.h>
#include <inviwo/core/datastructures/image/layerramprecision.h>
#include <inviwo/core/datastructures/histogram.h>

#include <inviwo/integrallinefiltering/datastructures/sparsehistogram.h>
#include <inviwo/integrallinefiltering/datastructures/directionalhistogram.h>

namespace inviwo {

/**
 * \namespace inviwo::entropy
 * Utility function to calculate the entropy of data.
 */
namespace entropy {
enum class PerformNormalization { Yes, No };

/**
 * Returns the maximum entropy value for a histogram of given size.
 * Used for normalization
 */
inline double shannonEntropyMax(size_t numberOfBins) {
    return std::log2(static_cast<double>(numberOfBins));
}

namespace detail {
template <typename T, typename = void>
struct is_pair : public std::false_type {};

template <typename T1, typename T2>
struct is_pair<std::pair<T1, T2>> : public std::true_type {};
}  // namespace detail

/**
 * Calculates the entropy for a given histogram of data.
 * For datasets that have not been binned into histogram use shannonEntropyScalars() for scalars,
 * shannonEntropyDirectional() for vector or shannonEntropyEuclidean() for points.
 * @see shannonEntropyScalars
 * @see shannonEntropyDirectional
 * @see shannonEntropyEuclidean
 */
template <typename Histogram>
double shannonEntropy(const Histogram& histogram) {
    size_t c = std::accumulate(histogram.begin(), histogram.end(), 0, [](size_t a, auto bin) {
        if constexpr (detail::is_pair<decltype(bin)>::value) {
            return a + bin.second;
        } else {
            return a + bin;
        }
    });
    double ent = 0;
    for (auto bin : histogram) {
        double h;
        if constexpr (detail::is_pair<decltype(bin)>::value) {
            h = static_cast<double>(bin.second);
        } else {
            h = static_cast<double>(bin);
        }
        if (h != 0) {  // log(0) = 0 when calculating entropy
            h /= c;
            ent += h * std::log2(h);
        }
    }

    return -ent;
}

/**
 * Calculates the entropy of the given input dataset. Bins the data into an histogram of \p numBins
 * @param data the input data
 * @param numBins number of bins to use for the histogram
 * @param normalize wether to use normalization or not, if normalization the output will be on the
 * range [0 1], if not, the range will be on [0 log2(numBins)]
 */
template <typename T>
double shannonEntropyScalars(const std::vector<T>& data, size_t numBins = 8,
                             PerformNormalization normalize = PerformNormalization::Yes) {
    auto minmax = std::minmax_element(data.begin(), data.end());
    T min = *minmax->first;
    T max = *minmax->second;

    SparseHistogram<size_t> histogram;

    for (const auto& v : data) {
        auto x = (v - min) / (max - min);
        auto i = static_cast<size_t>(x * (numBins - 1));
        if (i >= numBins) {
            i = numBins - 1;
        }
        histogram[i]++;
    }
    if (normalize == PerformNormalization::Yes) {
        return shannonEntropy(histogram) / shannonEntropyMax(histogram.numberOfBins());
    } else {
        return shannonEntropy(histogram);
    }
}

/**
 * Calculates the entropy of the given input dataset of directional vectors, for example velocities
 * in a vectorfield. Bins the data into an histogram of \p numBins. @see DirectionalHistogram
 * @param data the input data
 * @param numBins number of bins to use for the histogram
 * @param normalize wether to use normalization or not, if normalization the output will be on the
 * range [0 1], if not, the range will be on [0 log2(numBins)]
 */
template <size_t Dims, typename T>
double shannonEntropyDirectional(const std::vector<glm::vec<Dims, T>>& data, const size_t numBins,
                                 PerformNormalization normalize = PerformNormalization::Yes) {
    static_assert(std::is_floating_point_v<T>);
    static_assert(Dims == 2 || Dims == 3);

    const auto histogram = histogram::calculateDirectionalHistogram(data, numBins);
    if (normalize == PerformNormalization::Yes) {
        return shannonEntropy(histogram) / shannonEntropyMax(histogram.size());
    } else {
        return shannonEntropy(histogram);
    }
}

/**
 * Calculates the entropy of the given input dataset of points.
 * Bins the data into an histogram of with bins of size \p binSize.
 * @param points the input data
 * @param numBins number of bins to use for the histogram
 * @param normalize wether to use normalization or not, if normalization the output will be on the
 * range [0 1], if not, the range will be on [0 log2(numBins)]
 */
template <size_t Dims, typename T>
double shannonEntropyEuclidean(const std::vector<glm::vec<Dims, T>>& points,
                               const glm::vec<Dims, T>& binSize,
                               PerformNormalization normalize = PerformNormalization::Yes) {
    static_assert(std::is_floating_point_v<T>);
    using bin_t = glm::vec<Dims, glm::i64>;
    SparseHistogram<bin_t> histogram;

    for (const auto& v : points) {
        const auto bin = static_cast<bin_t>(glm::ceil(v / binSize));
        histogram[bin]++;
    }

    SparseHistogram<size_t> binHist;
    for (auto bin : histogram) {
        binHist[bin.second]++;
    }

    if (normalize == PerformNormalization::Yes) {
        return shannonEntropy(histogram) / shannonEntropyMax(histogram.numberOfBins());
    } else {
        return shannonEntropy(histogram);
    }
}

/**
 * Calculates the entropy of the given input dataset of points.
 * Bins the data into an histogram of with bins of size \p binSize.
 * @param points the input data
 * @param numBins number of bins to use for the histogram
 * @param normalize wether to use normalization or not, if normalization the output will be on the
 * range [0 1], if not, the range will be on [0 log2(numBins)]
 */
template <size_t Dims, typename T>
double shannonEntropyEuclidean(const std::vector<glm::vec<Dims, T>>& values, const T& binSize,
                               PerformNormalization normalize = PerformNormalization::Yes) {
    return shannonEntropyEuclidean(values, glm::vec<Dims, T>{binSize}, normalize);
}

}  // namespace entropy

}  // namespace inviwo

#endif  // IVW_SHANNONENTROPY2D_H
