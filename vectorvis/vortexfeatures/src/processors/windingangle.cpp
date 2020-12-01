/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/vortexfeatures/processors/windingangle.h>
#include <inviwo/vortexfeatures/algorithms/vortexutils.h>
#include <inviwo/core/util/spatialsampler.h>
#include <inviwo/core/util/imagesampler.h>
#include <inviwo/core/util/imageramutils.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <modules/vectorfieldvisualization/integrallinetracer.h>
#include <glm/gtx/vector_angle.hpp>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo WindingAngle::processorInfo_{
    "org.inviwo.WindingAngle",  // Class identifier
    "Winding Angle",            // Display name
    "Vortex Features",          // Category
    CodeState::Experimental,    // Code state
    Tags::None,                 // Tags
};
const ProcessorInfo WindingAngle::getProcessorInfo() const { return processorInfo_; }

WindingAngle::WindingAngle()
    : PoolProcessor(pool::Option::DelayDispatch)
    , inImage_("inImage", true)
    , linesOut_("linesOut")
    , vortexOut_("outVortices")
    , properties_("properties", "Batch Properties")
    , maxTotalSteps_("maxTotalSteps", "Max Total Steps", 1000, 100, 10000, 10)
    , samplingStep_("samplingStep", "Sampling Step", 3, 1, 10)
    , relativeThreshold_("relativeThresh", "Relative Threshold?", false)
    , relativeDistanceThreshold_("relThresh", "Relative End Threshold", 0.01, 0.0001, 0.1, 0.0001)
    , absoluteDistanceThreshold_("absThresh", "Absolute End Threshold", 0.1, 0.001, 1)
    , limitRadiusRatio_("limitShape", "Limit Radius Ratio", true)
    , maxRadiusRatio_("relativeRadiusThreshold", "Max Radius Ratio", 2.0, 1.0, 20.0, 0.01f)
    , roundCurves_("roundCurves", "Round Curves", true)
    , scoreProperties_("scoreProps", "Compute Vortex Score") {

    addPort(inImage_);
    addPort(vortexOut_);
    addPort(linesOut_);
    addProperties(properties_, maxTotalSteps_, samplingStep_, relativeThreshold_,
                  relativeDistanceThreshold_, absoluteDistanceThreshold_, limitRadiusRatio_,
                  maxRadiusRatio_, scoreProperties_, roundCurves_);

    properties_.stepDirection_.set(IntegralLineProperties::Direction::FWD);
    properties_.stepDirection_.setVisible(false);
    properties_.seedPointsSpace_.set(CoordinateSpace::Data);
    properties_.seedPointsSpace_.setVisible(false);

    auto turnOnIntegrate = [this]() { integrate_ = true; };
    auto turnOnFilter = [this]() { filter_ = true; };
    auto updateScoresLambda = [&]() { updateScores(); };

    inImage_.onChange(turnOnIntegrate);
    properties_.onChange(turnOnIntegrate);
    maxTotalSteps_.onChange(turnOnIntegrate);
    relativeThreshold_.onChange(turnOnFilter);
    relativeDistanceThreshold_.onChange(turnOnFilter);
    absoluteDistanceThreshold_.onChange(turnOnFilter);
    limitRadiusRatio_.onChange(turnOnFilter);
    maxRadiusRatio_.onChange(turnOnFilter);
    roundCurves_.onChange(turnOnFilter);

    scoreProperties_.onChange(updateScoresLambda);
    scoreProperties_.sizeWeight_.onChange(updateScoresLambda);
    scoreProperties_.roundnessWeight_.onChange(updateScoresLambda);
}

void WindingAngle::process() {
    relativeDistanceThreshold_.setVisible(relativeThreshold_);
    absoluteDistanceThreshold_.setVisible(!relativeThreshold_);
    maxRadiusRatio_.setVisible(limitRadiusRatio_);

    if (!inImage_.hasData()) return;
    if (integrate_) {
        integrateLines();
    }
    if (filter_) {
        filterLines();
    }
}

IntegralLine* WindingAngle::windStreamline(const StreamLine2DTracer& tracer, const dvec2& seed,
                                           size_t maxTotalSteps) {
    // Assemble winding angle.
    double windingAngle = 0;
    auto terminator = IntegralLine::TerminationReason::Steps;
    bool isClosed = false;

    IntegralLine* fullLine = new IntegralLine();

    std::vector<dvec3>& fullPoints = fullLine->getPositions();

    IntegralLine line = tracer.traceFrom(seed);
    std::vector<dvec3>& points = line.getPositions();
    auto& velocities = fullLine->getMetaData<dvec3>("velocity", true);
    velocities.reserve(points.size());

    if (points.size() < 1) {
        delete fullLine;
        return nullptr;
    }

    do {
        velocities.push_back({0, 0, 0});
        for (size_t idx = 1; idx < points.size() - 1; ++idx) {
            const dvec2 pos = points[idx];
            const dvec2 before = points[idx - 1];
            const dvec2 after = points[idx + 1];
            double angle =
                glm::orientedAngle(glm::normalize(after - pos), glm::normalize(pos - before));
            windingAngle += angle;
            velocities.push_back({std::abs(windingAngle), 0, 0});

            // Streamline fulfilled a circle.
            if (std::abs(windingAngle) >= 2.0 * M_PI) {
                isClosed = true;
                fullPoints.insert(fullPoints.end(), points.begin(), points.begin() + idx + 1);
                // Use the line index to store the rotation direction.
                fullLine->setIndex(
                    static_cast<size_t>(((windingAngle > 0) ? Vortex::Turning::Clockwise
                                                            : Vortex::Turning::CounterClockwise)));
                return fullLine;
            }
        }

        fullPoints.insert(fullPoints.end(), points.begin(), points.end() - 2);

        line = tracer.traceFrom(points.back());
        // One of { StartPoint, Steps, OutOfBounds, ZeroVelocity, Unknown }
        terminator = line.getForwardTerminationReason();
    } while (terminator == IntegralLine::TerminationReason::Steps && !isClosed &&
             fullPoints.size() < size_t(maxTotalSteps));

    delete fullLine;
    return nullptr;
}

void WindingAngle::integrateLines() {
    integrate_ = false;
    filter_ = false;

    if (!inImage_.hasData()) return;

    auto sampler = std::make_shared<ImageSpatialSampler<2, double>>(inImage_.getData());

    const auto integrator = [this, image = inImage_.getData(), sampler,
                             tracerConst = StreamLine2DTracer(sampler, properties_),
                             maxTotalSteps = maxTotalSteps_.get()](
                                pool::Stop stop,
                                pool::Progress progress) -> std::shared_ptr<IntegralLineSet> {
        if (stop) return nullptr;
        StreamLine2DTracer tracer(tracerConst);
        size2_t imgSize = image->getDimensions();
        vec2 invDim = vec2(1.0f) / vec2(imgSize);
        const LayerRAM* layer = image->getColorLayer()->getRepresentation<LayerRAM>();

        auto allLines =
            std::make_shared<IntegralLineSet>(sampler->getModelMatrix(), sampler->getWorldMatrix());

        std::mutex lineWriteMutex;

        size_t sampledPortion = samplingStep_.get() * samplingStep_.get();
        util::forEachPixelParallel(*layer, [&](size2_t px) {
            if (layer->getAsDouble(px) == 0 || px.x % samplingStep_.get() != 0 ||
                px.y % samplingStep_.get() != 0)
                return;

            dvec2 seed((0.5 + px.x) * invDim.x, (0.5 + px.y) * invDim.y);
            IntegralLine* line = windStreamline(tracer, seed, maxTotalSteps);

            if (line && line->getPositions().size() > 3) {
                line->getLength();
                {
                    std::lock_guard<std::mutex> lock(lineWriteMutex);
                    allLines->push_back(std::move(*line), IntegralLineSet::SetIndex::No);
                    progress(allLines->size(), imgSize.x * imgSize.y / sampledPortion);
                }
                delete line;
            }
        });
        return allLines;
    };

    // Dispatch the calculation to the thread pool. The second argument is a callback
    // that will get evaluated on the main thread after the calculation is done. The callback should
    // take the result of the calculation as argument.
    dispatchOne(integrator, [this](std::shared_ptr<IntegralLineSet> result) {
        allLines_ = result;
        linesOut_.setData(allLines_);

        filter_ = true;
        integrate_ = false;
        invalidate(InvalidationLevel::InvalidOutput);
    });
}

bool WindingAngle::filterLines() {
    filter_ = false;
    vortices_ =
        std::make_shared<VortexSet>(allLines_->getModelMatrix(), allLines_->getWorldMatrix());
    if (!inImage_.hasData() || allLines_->size() == 0) {
        vortexOut_.setData(vortices_);
        return false;
    }

    // Parallel filtering.
    std::mutex lineWriteMutex;
    util::forEachParallel(*allLines_, [&](const IntegralLine& line, size_t id) {
        const std::vector<dvec3>& points = line.getPositions();

        double endPointDist = glm::distance(points[0], points.back());
        bool takeLine = (!relativeThreshold_ && endPointDist < absoluteDistanceThreshold_.get());
        takeLine = takeLine || (relativeThreshold_ &&
                                endPointDist < relativeDistanceThreshold_ * line.getLength());
        if (takeLine) {
            // Sample lines for center.
            static constexpr size_t SAMPLE_STEP = 4;
            size_t numSamples =
                points.size() / SAMPLE_STEP + ((points.size() % SAMPLE_STEP) ? 1 : 0);
            dvec3 center(0);

            for (size_t p = 0; p < points.size(); p += SAMPLE_STEP) {
                center += points[p];
            }
            center /= numSamples;

            // Sample same points again for average radius and extreme radii.
            double endRadius = glm::distance(points.back(), center);
            double avgRadius = endRadius;
            double minRadius = endRadius;
            double maxRadius = endRadius;

            for (size_t p = 0; p < points.size(); p += SAMPLE_STEP) {
                double rad = glm::distance(points[p], center);
                avgRadius += rad;
                minRadius = std::min(minRadius, rad);
                maxRadius = std::max(maxRadius, rad);
            }
            avgRadius /= numSamples + 1;
            if (limitRadiusRatio_.get() && (maxRadius / minRadius) > maxRadiusRatio_.get()) {
                return;
            }
            Vortex::Turning rotation = (static_cast<Vortex::Turning>(line.getIndex()));
            // Passed all filters, select line.
            Vortex vort(line, center, avgRadius, minRadius, maxRadius, 42, 0, rotation);
            if (!roundCurves_.get() || vort.roundUpBoundary()) {
                std::lock_guard<std::mutex> lock(lineWriteMutex);
                vortices_->push_back(std::move(vort));
            }
        }
    });

    if (vortices_->size() <= 0) {
        vortexOut_.setData(vortices_);
        return false;
    }

    *vortices_ = vortexutil::groupVorticesByCenter(*vortices_);
    // *vortices_ = vortexutil::removeSurroundingVortices(*vortices_);

    updateScores();
    vortexOut_.setData(vortices_);
    return true;
}

void WindingAngle::updateScores() {
    if (!vortices_ || vortices_->size() == 0 || !scoreProperties_) return;
    scoreProperties_.computeScores(*vortices_);
}

WindingAngle::ScoreProperty::ScoreProperty(const std::string& identifier,
                                           const std::string& displayName)
    : BoolCompositeProperty(identifier, displayName, true)
    , roundnessWeight_("roundnessWeight", "Roundness Weight", 0.5, 0, 1, 0.01)
    , sizeWeight_("sizeWeight", "Size Weight", 0.5, 0, 1, 0.01)
    , maxSize_("maxSize", "Max Size", 3, {0, ConstraintBehavior::Ignore},
               {10, ConstraintBehavior::Ignore})
    , accumMaxSize_("accumMaxSize", "Data Max Size", 0, {0, ConstraintBehavior::Ignore},
                    {0, ConstraintBehavior::Ignore}, 0, InvalidationLevel::Valid,
                    PropertySemantics::Text) {
    addProperties(roundnessWeight_, sizeWeight_, maxSize_, accumMaxSize_);
    accumMaxSize_.setReadOnly(true);
}

double WindingAngle::ScoreProperty::getScore(const Vortex& vortex) const {
    double roundness = vortex.minRadius / vortex.maxRadius;  // 0 < q < 1, larger better
    double size = vortex.avgRadius / *maxSize_;              // 0 < q < 1, larger better
    return (roundness * roundnessWeight_) + (size * sizeWeight_);
}

void WindingAngle::ScoreProperty::computeScores(const VortexSet& vortices) {
    double accMaxSize = *accumMaxSize_;
    for (const Vortex& vort : vortices) {
        vort.score = getScore(vort);
        accMaxSize = (std::max(accMaxSize, vort.avgRadius));
    }
    accumMaxSize_.setReadOnly(false);
    accumMaxSize_.set(accMaxSize);
    accumMaxSize_.setReadOnly(true);
    vortices.updateScoreRange(vortices.size());
}

}  // namespace inviwo
