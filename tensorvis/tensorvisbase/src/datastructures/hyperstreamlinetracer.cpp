#include <inviwo/tensorvisbase/datastructures/hyperstreamlinetracer.h>

namespace inviwo {
HyperStreamLineTracer::HyperStreamLineTracer(
    std::shared_ptr<const SpatialSampler<3, double>> sampler,
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

    bool both = dir_ == IntegralLineProperties::Direction::Bidirectional;
    bool fwd = both || dir_ == IntegralLineProperties::Direction::Forward;
    bool bwd = both || dir_ == IntegralLineProperties::Direction::Backward;

    size_t stepsFWD = 0;
    size_t stepsBWD = 0;

    if (both) {
        stepsBWD = steps_ / 2;
        stepsFWD = steps_ - stepsBWD;
    } else if (fwd) {
        line.setBackwardTerminationReason(IntegralLine::TerminationReason::StartPoint);
        stepsFWD = steps_;
    } else if (bwd) {
        line.setForwardTerminationReason(IntegralLine::TerminationReason::StartPoint);
        stepsBWD = steps_;
    }

    stepsBWD++;  // for adjendency info
    stepsFWD++;

    line.getPositions().reserve(steps_ + 2);
    line.getMetaData<dvec3>("velocity", true).reserve(steps_ + 2);

    for (auto& m : metaSamplers_) {
        line.getMetaData<typename SpatialSampler<3, double>::ReturnType>(m.first, true)
            .reserve(steps_ + 2);
    }

    if (!addPoint(line, p)) {
        return res;  // Zero velocity at seed point
    }

    line.setBackwardTerminationReason(integrate(stepsBWD, p, line, false));

    if (!line.getPositions().empty()) {
        line.reverse();
        res.seedIndex = line.getPositions().size() - 1;
    }

    line.setForwardTerminationReason(integrate(stepsFWD, p, line, true));

    return res;
}

void HyperStreamLineTracer::addMetaDataSampler(
    const std::string& name, std::shared_ptr<const SpatialSampler<3, double>> sampler) {
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
        line.getMetaData<typename SpatialSampler<3, double>::ReturnType>(m.first).emplace_back(
            util::glm_convert<dvec3>(m.second->sample(pos)));
    }
    return true;
}

IntegralLine::TerminationReason HyperStreamLineTracer::integrate(size_t steps, const dvec3& pos,
                                                                 IntegralLine& line, bool fwd) {
    if (steps == 0) return IntegralLine::TerminationReason::StartPoint;

    DataVector worldVelocity;
    bool flipped{false};

    for (size_t i = 0; i < steps; i++) {
        if (!sampler_->withinBounds(pos)) {
            return IntegralLine::TerminationReason::OutOfBounds;
        }

        dvec3 newpos{0.0};
        std::tie(newpos, worldVelocity, flipped) =
            detail::hyperstep<DataVector>(pos, integrationScheme_, stepSize_ * (fwd ? 1.0 : -1.0),
                                          invBasis_, normalizeSamples_, *sampler_, flipped);

        if (!addPoint(line, newpos, worldVelocity)) {
            return IntegralLine::TerminationReason::ZeroVelocity;
        }
    }

    return IntegralLine::TerminationReason::Steps;
}
}  // namespace inviwo