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
    , outLines_("outLines")
    , outAllLines_("outAllLines")
    , outCenters_("outCenters")

    , properties_("properties", "Batch Properties")
    , relativeThreshold_("relativeThresh", "Relative Threshold?", false)
    , relativeDistanceThreshold_("relThresh", "Relative End Threshold", 0.01, 0.0001, 0.1, 0.0001)
    , absoluteDistanceThreshold_("absThresh", "Absolute End Threshold", 0.1, 0.001, 1) {

    addPort(inImage_);
    addPort(outLines_);
    addPort(outAllLines_);
    addPort(outCenters_);
    addProperties(properties_, relativeThreshold_, relativeDistanceThreshold_,
                  absoluteDistanceThreshold_);

    properties_.stepDirection_.set(IntegralLineProperties::Direction::FWD);
    properties_.stepDirection_.setVisible(false);
    properties_.seedPointsSpace_.set(CoordinateSpace::Data);
    properties_.seedPointsSpace_.setVisible(false);

    inImage_.onChange([this]() { integrateLines(); });
    properties_.onChange([this]() { integrateLines(); });
}

void WindingAngle::integrateLines() {
    relativeDistanceThreshold_.setVisible(relativeThreshold_);
    absoluteDistanceThreshold_.setVisible(!relativeThreshold_);

    if (!inImage_.hasData()) return;

    // const auto calc = [image = inImage_.getData(), properties = properties_.get()](
    //                       pool::Stop stop,
    //                       pool::Progress progress) -> std::shared_ptr<const IntegralLineSet> {};

    size2_t imgSize = inImage_.getData()->getDimensions();
    vec2 invDim = vec2(1.0f) / vec2(imgSize);
    const LayerRAM* layer = inImage_.getData()->getColorLayer()->getRepresentation<LayerRAM>();

    auto sampler = std::make_shared<ImageSpatialSampler<2, double>>(inImage_.getData());
    allLines_ =
        std::make_shared<IntegralLineSet>(sampler->getModelMatrix(), sampler->getWorldMatrix());

    StreamLine2DTracer tracer(sampler, properties_);

    std::mutex lineWriteMutex;

    std::mutex tmpCellCountMutex;
    size_t tmpNumCells = 0;  // 35372 cells
    util::forEachPixel(
        *layer,
        [&](size2_t px) {  // TODO: Parallel!
            if (layer->getAsDouble(px) == 0) return;

            // Assemble winding angle.
            double windingAngle = 0;
            auto terminator = IntegralLine::TerminationReason::Steps;
            bool isClosed = false;

            IntegralLine fullLine;

            // int tmpId = 0;
            std::vector<dvec3>& fullPoints = fullLine.getPositions();
            std::vector<dvec3>& fullVelocities = fullLine.getMetaData<dvec3>("velocity", true);
            // fullVelocities.reserve(properties_.numberOfSteps_ + 2);
            // {
            //     std::lock_guard<std::mutex> tmpLock(tmpCellCountMutex);

            //     tmpNumCells++;
            //     tmpId = tmpNumCells;
            //     if (tmpNumCells > 10) {
            //         return;
            //     }
            //     //  LogWarn("Here!");
            // }
            // LogWarn(dvec2(px.x, px.y) + 0.5);
            IntegralLine line =
                tracer.traceFrom(dvec2((0.5 + px.x) / imgSize.x, (0.5 + px.y) / imgSize.y));
            std::vector<dvec3>& points = line.getPositions();
            std::vector<dvec3>& velocities = line.getMetaData<dvec3>("velocity", true);

            if (points.size() < 1) {
                return;
            }

            do {
                for (int idx = 1; idx < points.size() - 1; ++idx) {
                    const dvec2 pos = points[idx];
                    const dvec2 before = points[idx - 1];
                    const dvec2 after = points[idx + 1];
                    double angle = glm::orientedAngle(glm::normalize(after - pos),
                                                      glm::normalize(pos - before));
                    windingAngle += angle;

                    // Streamline fulfilled a circle.
                    if (std::abs(windingAngle) > 360) {  // M_PI_2) {
                        isClosed = true;
                        fullPoints.insert(fullPoints.end(), points.begin(),
                                          points.begin() + idx + 1);
                        fullVelocities.insert(fullVelocities.end(), velocities.begin(),
                                              velocities.begin() + idx + 1);
                        break;
                    }
                }
                if (isClosed) {
                    // TODO: Find center.
                    if (fullPoints.size() > 3) {
                        std::lock_guard<std::mutex> lock(lineWriteMutex);
                        allLines_->push_back(std::move(fullLine), IntegralLineSet::SetIndex::No);
                    }
                    return;
                }

                fullPoints.insert(fullPoints.end(), points.begin(), points.end());
                fullVelocities.insert(fullVelocities.end(), velocities.begin(), velocities.end());

                // std::cout << tmpId << " - " << fullPoints.size() << std::endl;
                // std::cout << tmpId << " - " << fullPoints.back() << std::endl;
                dvec2 seed = util::glm_convert<dvec2>(fullLine.getPositions().back());
                // std::cout << tmpId << " - " << seed << std::endl;
                line = tracer.traceFrom(fullLine.getPositions().back());
                // One of { StartPoint, Steps, OutOfBounds, ZeroVelocity, Unknown }
                terminator = line.getForwardTerminationReason();
            } while (terminator == IntegralLine::TerminationReason::Steps && !isClosed);
            // LogWarn("# Points " << fullPoints.size());

        });
    outAllLines_.setData(allLines_);
    invalidate(InvalidationLevel::InvalidOutput);
}

void WindingAngle::process() {
    if (!inImage_.hasData()) return;

    // const auto calc = [image = inImage_.getData(), properties = properties_.get()](
    //                       pool::Stop stop,
    //                       pool::Progress progress) -> std::shared_ptr<const IntegralLineSet> {};
    auto selectedLines =
        std::make_shared<IntegralLineSet>(allLines_->getModelMatrix(), allLines_->getWorldMatrix());

    LogWarn("# Lines = " << allLines_->size());
    int numDist = 0;
    double avgDist = 0;
    for (size_t lIdx = 0; lIdx < allLines_->size(); ++lIdx) {
        auto& line = allLines_->at(lIdx);
        std::vector<dvec3>& points = line.getPositions();
        std::vector<dvec3>& velocities = line.getMetaData<dvec3>("velocity", true);

        double endPointDist = glm::distance(points[0], points.back());
        avgDist += endPointDist / allLines_->size();
        if (lIdx % 10000 == 0) LogWarn("Dist at " << lIdx << ": " << endPointDist);
        numDist++;
        bool takeLine =
            (!relativeThreshold_ && endPointDist < absoluteDistanceThreshold_.get());  // TODO
        takeLine = takeLine || (relativeThreshold_ &&
                                endPointDist < relativeDistanceThreshold_ * line.getLength());
        if (takeLine) {
            std::fill(velocities.begin(), velocities.end(), dvec3(endPointDist, 0, 0));
            selectedLines->push_back(IntegralLine(line), IntegralLineSet::SetIndex::Yes);
            // for (int idx = 0; idx < points.size(); ++idx)
            // velocities[idx] = dvec3(endPointDist, 0, 0);
        }
    }
    outLines_.setData(selectedLines);
    LogWarn("Avg. end point distance: " << avgDist);
    LogWarn("\tOf " << numDist << " lines.");
    LogWarn("===> NUM SELCTED LINES <=== " << selectedLines->size());
}
}  // namespace inviwo
