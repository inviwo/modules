/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2021 Inviwo Foundation
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
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/ports/imageport.h>

namespace inviwo {
namespace detail {

/**
 * Calculates the area of a cap of cangle /p colatitudeAngle of a sphere with radius 1
 */
template <typename T>
T sphericalCapArea(const T colatitudeAngle) {
    /*
     * The forumula in the paper is $4*pi*sin^2(colatitudeAngle/2)$, but applyingthe half-angle
     * formula the we get rid of the square and end up with: $2*pi*(1-cos(colatitudeAngle))$
     */
    return glm::two_pi<T>() * (1 - std::cos(colatitudeAngle));
}

/**
 * Inverse of sphericalCapArea(colatitudeAngle). Calcualtes the colatitudal angle for a spherical
 * cap with a given \p sphericalCapArea on a sphere with a radius of 1.
 */
template <typename T>
T colatitudeAngle(const T sphericalCapArea) {
    return std::acos(1 - sphericalCapArea / (glm::two_pi<T>()));
}

}  // namespace detail

/**
 * \brief class represeting a sphere diveded into N uniform pathces
 * UniformSpherePartitioning is a classed that partition the sphere into N pathces. Each patch has
 * equal area and small diameter. Algorithm is based on [1].
 *
 * \image html sphere-partitioning.png "Image demostrating 100k points binned using a
 * directional histogram with 100 bins"
 *
 * [1] Leopardi, Paul. "A partition of the unit sphere into regions of equal area and small
 * diameter." Electronic Transactions on Numerical Analysis 25.12 (2006): 309-327.
 */
template <typename T>
class UniformSpherePartitioning {
    static_assert(std::is_floating_point_v<T>);
    struct Collar {
        Collar(T endAngle, size_t patches)
            : cosAngle_(cos(endAngle)), patches_(patches), startIndex(0) {}

        const T cosAngle_;
        const size_t patches_;
        size_t startIndex;

        size_t index(const T x, const T y) const {
            if (patches_ == 1) {
                return startIndex;
            }
            const T alpha = atan2(y, x) / glm::two_pi<T>() + T(0.5);
            const auto A = std::min(static_cast<size_t>(alpha * patches_), patches_ - 1);
            return A + startIndex;
        }

        bool operator<(const T cosAngle) const { return this->cosAngle_ > cosAngle; }
    };

public:
    /**
     * Crate a sphere partition with given number of patches.
     */
    UniformSpherePartitioning(const size_t segments = 20) {
        if (segments == 0) {
            throw Exception("Zero segments not allowed", IVW_CONTEXT);
        } else if (segments == 1) {
            collars_.emplace_back(glm::pi<T>(), 1);
            return;
        } else if (segments == 2) {
            collars_.emplace_back(glm::half_pi<T>(), 1);
            collars_.emplace_back(glm::pi<T>(), 1);
            collars_.back().startIndex = 1;
            return;
        }

        const T regionArea = 4 * glm::pi<T>() / segments;
        const T colatPole = detail::colatitudeAngle(regionArea);

        const T idealCollarAngle = std::sqrt(regionArea);
        const T idealNumberOfCollars = (glm::pi<T>() - 2 * colatPole) / idealCollarAngle;

        const size_t numberOfCollars =
            std::max(size_t(1), static_cast<size_t>(idealNumberOfCollars + 0.5));

        const auto fittingCollarAngle = (glm::pi<T>() - 2 * colatPole) / numberOfCollars;

        auto collatOfCollar = [&](size_t i) { return colatPole + (i - 1) * fittingCollarAngle; };

        std::vector<T> idealNumberOfRegions;
        idealNumberOfRegions.push_back(0);
        for (size_t i = 1; i <= numberOfCollars; i++) {
            const auto a = detail::sphericalCapArea(collatOfCollar(i + 1));
            const auto b = detail::sphericalCapArea(collatOfCollar(i));
            idealNumberOfRegions.push_back((a - b) / regionArea);
        }

        std::vector<T> accumulatedMissingRegionArea(1, 0);  //
        std::vector<size_t> regionsInCollar(1, 0);
        for (size_t i = 1; i <= numberOfCollars; i++) {
            T tmp = idealNumberOfRegions[i] + accumulatedMissingRegionArea[i - 1];
            regionsInCollar.push_back(static_cast<size_t>(tmp + 0.5));
            accumulatedMissingRegionArea.push_back(T(0.0));
            for (size_t j = 1; j <= i; j++) {
                accumulatedMissingRegionArea.back() += idealNumberOfRegions[j] - regionsInCollar[j];
            }
        }

        std::vector<T> collatitudes;
        collatitudes.push_back(0);
        for (size_t i = 1; i <= numberOfCollars + 1; i++) {
            T totM = 0;
            for (size_t j = 1; j < i; j++) {
                totM += regionsInCollar[j];
            }
            const T v = (1 + totM) * regionArea;
            collatitudes.push_back(detail::colatitudeAngle(v));
        }
        collatitudes.push_back(glm::pi<T>());

        collars_.emplace_back(collatitudes[1], 1);
        for (size_t i = 1; i < collatitudes.size() - 2; i++) {
            collars_.emplace_back(collatitudes[i + 1], regionsInCollar[i]);
        }
        collars_.emplace_back(glm::pi<T>(), 1);

        size_t count = 0;
        for (auto &segment : collars_) {
            segment.startIndex = count;
            count += segment.patches_;
        }

        IVW_ASSERT(count == segments,
                   "Code failed to partion the sphere into the right number of segments");
    }

    UniformSpherePartitioning(const UniformSpherePartitioning &) = default;
    UniformSpherePartitioning(UniformSpherePartitioning &&) = default;
    UniformSpherePartitioning &operator=(const UniformSpherePartitioning &) = default;
    UniformSpherePartitioning &operator=(UniformSpherePartitioning &&) = default;

    /**
     * Returns the number of partitions.
     */
    size_t numberOfPatches() const { return collars_.back().startIndex + collars_.back().patches_; }

    /**
     * Return the index of the partition for the given direction.
     */
    size_t getRegionForDirection(const glm::vec<3, T> &in_dir) const {
        const auto dir = glm::normalize(in_dir);
        const auto it = std::lower_bound(collars_.begin(), collars_.end(), dir.z);
        return it->index(dir.x, dir.y);
    }

private:
    std::vector<Collar> collars_;
};

}  // namespace inviwo
