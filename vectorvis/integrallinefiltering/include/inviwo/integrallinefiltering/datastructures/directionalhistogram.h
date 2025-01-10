/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2025 Inviwo Foundation
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

#include <inviwo/integrallinefiltering/integrallinefilteringmoduledefine.h>
#include <inviwo/core/util/glm.h>
#include <inviwo/core/util/foreach.h>

#include <inviwo/integrallinefiltering/algorithm/uniformspherepartitioning.h>

namespace inviwo {

namespace histogram {

/**
 * Funcion to compute a histgram of a set of 2D vectors. Only their
 * direction is considered, magnitudes are ignored. The histogram consits of N bins, each
 * representing a unique secotor of the circle with a central angle of 2*pi/N.
 */
template <typename T>
const std::vector<size_t> calculateDirectionalHistogram(
    const std::vector<glm::vec<2, T>>& directions, const size_t numberOfBins = 20) {
    static_assert(std::is_floating_point_v<T>);
    std::vector<size_t> bins(numberOfBins, 0);
    for (const auto& dir : directions) {
        const auto a = atan2(dir.y, dir.x) / glm::two_pi<T>() + 0.5;
        const auto I = std::min(static_cast<size_t>(a * numberOfBins), numberOfBins - 1);
        bins[I]++;
    }
    return bins;
}

/**
 * Funcion to compute a histgram of a set of 3D vectors. Only their
 * direction is considered, magnitudes are ignored. The function uses UniformSpherePartitioning
 * to crate a histogram with N bins of equal area and diameter.
 * @see UniformSpherePartitioning
 */
template <typename T>
const std::vector<size_t> calculateDirectionalHistogram(
    const std::vector<glm::vec<3, T>>& directions, const size_t numberOfBins = 20) {
    static_assert(std::is_floating_point_v<T>);

    UniformSpherePartitioning<T> partions(numberOfBins);
    std::vector<size_t> bins(numberOfBins, 0);
    for (const auto& dir : directions) {
        bins[partions.getRegionForDirection(dir)]++;
    }
    return bins;
}

}  // namespace histogram

}  // namespace inviwo
