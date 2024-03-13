/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2024 Inviwo Foundation
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

#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <modules/vectorfieldvisualization/datastructures/integralline.h>
#include <modules/vectorfieldvisualization/properties/integrallineproperties.h>
#include <inviwo/core/util/spatialsampler.h>
#include <inviwo/core/util/glmvec.h>
#include <inviwo/core/util/glmmat.h>
#include <inviwo/core/util/glmutils.h>

namespace inviwo {

namespace detail {

template <typename V, typename H, typename M, typename P>
P seedTransform(const M& m, const P& pIn) {
    auto p = m * H(pIn, 1.0f);
    return P(p) / p[util::extent_v<P>];
}

template <typename DataVector, typename Sampler, typename F, typename DataMatrix>
std::tuple<dvec3, DataVector, bool> hyperstep(const dvec3& oldPos,
                                              IntegralLineProperties::IntegrationScheme, F stepSize,
                                              const DataMatrix& invBasis, bool normalizeSamples,
                                              const Sampler& sampler, const bool prevFlipped) {

    auto normalize = [](auto v) {
        auto l = glm::length(v);
        if (l == 0) return v;
        return v / l;
    };

    auto move = [&](auto pos, auto v, auto stepsize) {
        if (normalizeSamples) {
            v = normalize(v);
        }
        auto offset = (invBasis * (v * stepsize));

        return pos + offset;
    };

    auto k1 = sampler.sample(oldPos);

    if (prevFlipped) k1 = -k1;

    bool flipped{false};

    auto k2 = sampler.sample(move(oldPos, k1, stepSize / 2));

    if (std::acos(glm::dot(k1, k2) / (glm::length(k1) * glm::length(k2))) < 90) k2 = -k2;

    auto k3 = sampler.sample(move(oldPos, k2, stepSize / 2));

    if (std::acos(glm::dot(k2, k3) / (glm::length(k2) * glm::length(k3))) < 90) k3 = -k3;

    auto k4 = sampler.sample(move(oldPos, k3, stepSize));

    if (std::acos(glm::dot(k3, k4) / (glm::length(k3) * glm::length(k4))) < 90) {
        k4 = -k4;
        flipped = true;
    }

    auto K = k1 + k2 + k2 + k3 + k3 + k4;

    if (normalizeSamples) {
        K = normalize(K);
    } else {
        K = K / 6.0;
    }

    return {move(oldPos, K, stepSize), k1, flipped};
}
}  // namespace detail

class IVW_MODULE_TENSORVISBASE_API HyperStreamLineTracer {
public:
    struct Result {
        IntegralLine line;
        size_t seedIndex{0};
        operator IntegralLine() const { return line; }
    };
    /*
     * Various types used within this class
     */
    using DataVector = dvec3;
    using DataHomogeneousVector = dvec4;
    using DataMatrix = dmat3;
    using DataHomogeneousSpatialMatrix = dmat4;

    HyperStreamLineTracer(std::shared_ptr<const SpatialSampler<dvec3>> sampler,
                          const IntegralLineProperties& properties);

    Result traceFrom(const dvec3& pIn);

    void addMetaDataSampler(const std::string& name,
                            std::shared_ptr<const SpatialSampler<dvec3>> sampler);

    const DataHomogeneousSpatialMatrix& getSeedTransformationMatrix() const;

    void setTransformOutputToWorldSpace(bool transform);
    bool isTransformingOutputToWorldSpace() const;

private:
    bool addPoint(IntegralLine& line, const dvec3& pos);
    bool addPoint(IntegralLine& line, const dvec3& pos, const DataVector& worldVelocity);

    IntegralLine::TerminationReason integrate(size_t steps, const dvec3& pos, IntegralLine& line,
                                              bool fwd);

    IntegralLineProperties::IntegrationScheme integrationScheme_;

    int steps_;
    double stepSize_;
    IntegralLineProperties::Direction dir_;
    bool normalizeSamples_;

    std::shared_ptr<const SpatialSampler<dvec3>> sampler_;
    std::unordered_map<std::string, std::shared_ptr<const SpatialSampler<dvec3>>> metaSamplers_;

    DataMatrix invBasis_;
    DataHomogeneousSpatialMatrix seedTransformation_;
    DataHomogeneousSpatialMatrix toWorld_;
    bool transformOutputToWorldSpace_;
};

}  // namespace inviwo
