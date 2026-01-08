/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2026 Inviwo Foundation
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

#include <inviwo/tensorvisbase/datastructures/hyperstreamlinetracer.h>

namespace inviwo {
HyperStreamLineTracer::HyperStreamLineTracer(std::shared_ptr<const SpatialSampler<dvec3>> sampler,
                                             const IntegralLineProperties& properties)
    : integrationScheme_(properties.getIntegrationScheme())
    , steps_(properties.getNumberOfSteps())
    , stepSize_(properties.getStepSize())
    , dir_(properties.getStepDirection())
    , normalizeSamples_(properties.getNormalizeSamples())
    , sampler_(sampler)
    , invBasis_(glm::inverse(DataMatrix(sampler->getModelMatrix())))
    , seedTransformation_(
          properties.getSeedPointTransformationMatrix(sampler->getCoordinateTransformer()))
    , toWorld_(sampler->getCoordinateTransformer().getDataToWorldMatrix())
    , transformOutputToWorldSpace_{false} {}

typename HyperStreamLineTracer::Result HyperStreamLineTracer::traceFrom(const dvec3& pIn) {
    dvec3 p = detail::seedTransform<DataVector, DataHomogeneousVector>(seedTransformation_, pIn);

    Result res;
    IntegralLine& line = res.line;

    const auto [stepsBWD, stepsFWD] = [dir = dir_, steps = steps_,
                                       &line]() -> std::pair<size_t, size_t> {
        switch (dir) {
            case inviwo::IntegralLineProperties::Direction::Forward:
                line.setBackwardTerminationReason(IntegralLine::TerminationReason::StartPoint);
                return {1, steps + 1};
            case inviwo::IntegralLineProperties::Direction::Backward:
                line.setForwardTerminationReason(IntegralLine::TerminationReason::StartPoint);
                return {steps + 1, 1};
            default:
            case inviwo::IntegralLineProperties::Direction::Bidirectional: {
                return {steps / 2 + 1, steps - (steps / 2) + 1};
            }
        }
    }();

    line.getPositions().reserve(steps_ + 2);
    line.getMetaData<dvec3>("velocity", true).reserve(steps_ + 2);

    for (auto& m : metaSamplers_) {
        line.getMetaData<typename SpatialSampler<dvec3>::type>(m.first, true).reserve(steps_ + 2);
    }

    if (!addPoint(line, p)) {
        return res;  // Zero velocity at seed point
    }

    line.setBackwardTerminationReason(integrate(stepsBWD, p, line, false));

    if (line.getPositions().size() > 1) {
        line.reverse();
        res.seedIndex = line.getPositions().size() - 1;
    }

    line.setForwardTerminationReason(integrate(stepsFWD, p, line, true));

    return res;
}

void HyperStreamLineTracer::addMetaDataSampler(
    const std::string& name, std::shared_ptr<const SpatialSampler<dvec3>> sampler) {
    metaSamplers_[name] = sampler;
}

auto HyperStreamLineTracer::getSeedTransformationMatrix() const
    -> const DataHomogeneousSpatialMatrix& {
    return seedTransformation_;
}

void HyperStreamLineTracer::setTransformOutputToWorldSpace(bool transform) {
    transformOutputToWorldSpace_ = transform;
}

bool HyperStreamLineTracer::isTransformingOutputToWorldSpace() const {
    return transformOutputToWorldSpace_;
}

bool HyperStreamLineTracer::addPoint(IntegralLine& line, const dvec3& pos) {
    return addPoint(line, pos, sampler_->sample(pos));
}

bool HyperStreamLineTracer::addPoint(IntegralLine& line, const dvec3& pos,
                                     const DataVector& worldVelocity) {

    if (glm::length(worldVelocity) < std::numeric_limits<double>::epsilon()) {
        return false;
    }

    if (transformOutputToWorldSpace_) {
        dvec3 worldPos = detail::seedTransform<DataVector, DataHomogeneousVector>(toWorld_, pos);

        line.getPositions().emplace_back(util::glm_convert<dvec3>(worldPos));
    } else {
        line.getPositions().emplace_back(util::glm_convert<dvec3>(pos));
    }

    line.getMetaData<dvec3>("velocity").emplace_back(util::glm_convert<dvec3>(worldVelocity));

    for (auto& m : metaSamplers_) {
        line.getMetaData<typename SpatialSampler<dvec3>::type>(m.first).emplace_back(
            util::glm_convert<dvec3>(m.second->sample(pos)));
    }
    return true;
}

IntegralLine::TerminationReason HyperStreamLineTracer::integrate(size_t steps, const dvec3& pos,
                                                                 IntegralLine& line, bool fwd) {
    if (steps == 0) return IntegralLine::TerminationReason::StartPoint;

    DataVector worldVelocity;
    bool flipped{false};

    dvec3 currentPos{pos};
    for (size_t i = 0; i < steps; i++) {
        if (!sampler_->withinBounds(pos)) {
            return IntegralLine::TerminationReason::OutOfBounds;
        }

        dvec3 newpos{0.0};
        std::tie(newpos, worldVelocity, flipped) = detail::hyperstep<DataVector>(
            currentPos, integrationScheme_, stepSize_ * (fwd ? 1.0 : -1.0), invBasis_,
            normalizeSamples_, *sampler_, flipped);
        currentPos = newpos;

        if (!addPoint(line, newpos, worldVelocity)) {
            return IntegralLine::TerminationReason::ZeroVelocity;
        }
    }

    return IntegralLine::TerminationReason::Steps;
}

}  // namespace inviwo
