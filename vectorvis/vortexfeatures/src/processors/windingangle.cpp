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
#include <inviwo/core/util/spatialsampler.h>
#include <inviwo/core/util/imagesampler.h>
#include <inviwo/core/util/imageramutils.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <modules/vectorfieldvisualization/integrallinetracer.h>
#include <glm/gtx/vector_angle.hpp>
#include <random>

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
    , outLines_("outLines")
    , outAllLines_("outAllLines")
    , outCenters_("outCenters")

    , properties_("properties", "Batch Properties")
    , maxTotalSteps_("maxTotalSteps", "Max Total Steps", 1000, 100, 10000, 10)
    , relativeThreshold_("relativeThresh", "Relative Threshold?", false)
    , relativeDistanceThreshold_("relThresh", "Relative End Threshold", 0.01, 0.0001, 0.1, 0.0001)
    , absoluteDistanceThreshold_("absThresh", "Absolute End Threshold", 0.1, 0.001, 1)
    , limitRadiusRatio_("limitShape", "Limit Radius Ratio", true)
    , maxRadiusRatio_("relativeRadiusThreshold", "Max Radius Ratio", 2.0, 1.0, 20.0, 0.01f) {

    addPort(inImage_);
    addPort(outLines_);
    addPort(outAllLines_);
    addPort(outCenters_);
    addProperties(properties_, maxTotalSteps_, relativeThreshold_, relativeDistanceThreshold_,
                  absoluteDistanceThreshold_, limitRadiusRatio_, maxRadiusRatio_);

    properties_.stepDirection_.set(IntegralLineProperties::Direction::FWD);
    properties_.stepDirection_.setVisible(false);
    properties_.seedPointsSpace_.set(CoordinateSpace::Data);
    properties_.seedPointsSpace_.setVisible(false);

    auto turnOnIntegrate = [this]() { integrate_ = true; };
    auto turnOnFilter = [this]() { filter_ = true; };
    inImage_.onChange(turnOnIntegrate);
    properties_.onChange(turnOnIntegrate);
    maxTotalSteps_.onChange(turnOnIntegrate);
    relativeThreshold_.onChange(turnOnFilter);
    relativeDistanceThreshold_.onChange(turnOnFilter);
    absoluteDistanceThreshold_.onChange(turnOnFilter);
    limitRadiusRatio_.onChange(turnOnFilter);
    maxRadiusRatio_.onChange(turnOnFilter);
}

void WindingAngle::process() {
    relativeDistanceThreshold_.setVisible(relativeThreshold_);
    absoluteDistanceThreshold_.setVisible(!relativeThreshold_);
    maxRadiusRatio_.setVisible(limitRadiusRatio_);

    if (!inImage_.hasData()) return;

    if (integrate_) {
        integrateLines();
    } else if (filter_) {
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
    std::vector<dvec3>& fullVelocities = fullLine->getMetaData<dvec3>("velocity", true);

    IntegralLine line = tracer.traceFrom(seed);
    std::vector<dvec3>& points = line.getPositions();
    std::vector<dvec3>& velocities = line.getMetaData<dvec3>("velocity", true);
    fullPoints.push_back(util::glm_convert<dvec3>(seed));
    fullVelocities.push_back(dvec3(0));

    if (points.size() < 1) {
        delete fullLine;
        return nullptr;
    }

    do {
        for (size_t idx = 1; idx < points.size() - 1; ++idx) {
            const dvec2 pos = points[idx];
            const dvec2 before = points[idx - 1];
            const dvec2 after = points[idx + 1];
            double angle =
                glm::orientedAngle(glm::normalize(after - pos), glm::normalize(pos - before));
            windingAngle += angle;

            // Streamline fulfilled a circle.
            if (std::abs(windingAngle) >= 2.0 * M_PI) {
                isClosed = true;
                fullPoints.insert(fullPoints.end(), points.begin(), points.begin() + idx);
                fullVelocities.insert(fullVelocities.end(), velocities.begin(),
                                      velocities.begin() + idx + 1);
                // Compute closest point on last edge.
                dvec2 edge = pos - before;
                dvec2 distToStart = seed - before;
                dvec2 endPoint = before + glm::dot(edge, distToStart) * glm::normalize(edge);
                fullPoints.push_back(util::glm_convert<dvec3>(endPoint));

                // Mesh output will be strip adjacency - repeat last point.
                fullPoints.push_back(fullPoints.back());
                fullVelocities.push_back(fullVelocities.back());
                return fullLine;
            }
        }

        fullPoints.insert(fullPoints.end(), points.begin(), points.end());
        fullVelocities.insert(fullVelocities.end(), velocities.begin(), velocities.end());

        line = tracer.traceFrom(fullLine->getPositions().back());
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

    const auto integrator = [image = inImage_.getData(), sampler,
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

        util::forEachPixelParallel(*layer, [&](size2_t px) {
            if (layer->getAsDouble(px) == 0 || px.x % 10 != 0 || px.y % 10 != 0) return;

            dvec2 seed((0.5 + px.x) * invDim.x, (0.5 + px.y) * invDim.y);
            IntegralLine* line = windStreamline(tracer, seed, maxTotalSteps);

            if (line && line->getPositions().size() > 3) {
                line->getLength();
                {
                    std::lock_guard<std::mutex> lock(lineWriteMutex);
                    allLines->push_back(std::move(*line), IntegralLineSet::SetIndex::No);
                    progress(allLines->size(), imgSize.x * imgSize.y / 100);
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

        filter_ = (result->size() > 0);
        integrate_ = false;
        invalidate(InvalidationLevel::InvalidOutput);
    });
}

bool WindingAngle::filterLines() {
    filter_ = false;
    if (!inImage_.hasData() || allLines_->size() == 0) return false;

    auto selectedLines =
        std::make_shared<IntegralLineSet>(allLines_->getModelMatrix(), allLines_->getWorldMatrix());

    // Lists of center points, and their estimated radii.
    std::vector<dvec3> centerPoints;
    std::vector<double> radii;
    size_t expectedNumFiltered = allLines_->size() * 0.1;
    centerPoints.reserve(expectedNumFiltered);
    radii.reserve(expectedNumFiltered);

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
            double endRadius = glm::distance2(points.back(), center);
            double avgRadius = endRadius;
            double minRadius = endRadius;
            double maxRadius = endRadius;

            for (size_t p = 0; p < points.size(); p += SAMPLE_STEP) {
                double rad = glm::distance2(points[p], center);
                avgRadius += rad;
                minRadius = std::min(minRadius, rad);
                maxRadius = std::max(maxRadius, rad);
            }
            avgRadius /= numSamples + 1;
            if (limitRadiusRatio_.get() && (maxRadius / minRadius) > maxRadiusRatio_.get()) {
                return;
            }

            // Passed all filters, select line.
            std::lock_guard<std::mutex> lock(lineWriteMutex);
            selectedLines->push_back(line, IntegralLineSet::SetIndex::Yes);
            centerPoints.push_back(center);
            radii.push_back(avgRadius);
        }
    });
    outAllLines_.setData(selectedLines);

    if (selectedLines->size() <= 0) return false;

    // Mesh setup, center clustering.
    std::vector<SphereMesh::Vertex> centerPointVerts;
    centerPointVerts.reserve(centerPoints.size());
    auto mesh = std::make_shared<SphereMesh>(DrawType::Points, ConnectivityType::None);

    auto outerLines =
        std::make_shared<IntegralLineSet>(allLines_->getModelMatrix(), allLines_->getWorldMatrix());
    std::vector<int> clusterId(selectedLines->size());

    // Random color generation.
    static std::default_random_engine eng;
    static std::uniform_real_distribution<float> rndDist(0, 1);
    for (size_t initIdx = 0; initIdx < selectedLines->size(); ++initIdx) {
        if (clusterId[initIdx] != 0) continue;
        vec4 color(rndDist(eng), rndDist(eng), rndDist(eng), 1);
        centerPointVerts.push_back({centerPoints[initIdx], 0.002, color});

        int id = centerPointVerts.size() + 1;
        size_t outerIdx = initIdx;
        clusterId[initIdx] = id;
        for (size_t neighIdx = initIdx; neighIdx < selectedLines->size(); ++neighIdx) {
            double dist = glm::distance2(centerPoints[outerIdx], centerPoints[neighIdx]);
            if (dist < std::max(radii[outerIdx], radii[neighIdx])) {
                centerPointVerts.push_back({centerPoints[neighIdx], 0.002, color});
                clusterId[neighIdx] = id;
                if (radii[neighIdx] > radii[outerIdx]) outerIdx = neighIdx;
            }
        }
        outerLines->push_back(selectedLines->at(outerIdx), IntegralLineSet::SetIndex::Yes);
        centerPointVerts.push_back({centerPoints[outerIdx], 0.004, color * 1.01});
    }
    mesh->addVertices(centerPointVerts);
    outCenters_.setData(mesh);
    outLines_.setData(outerLines);

    return true;
}
}  // namespace inviwo
