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

#include <inviwo/vortexfeatures/processors/floodfillimage.h>
#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/datastructures/image/layer.h>
#include <inviwo/core/datastructures/image/layerram.h>
#include <inviwo/core/datastructures/image/layerramprecision.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo FloodfillImage::processorInfo_{
    "org.inviwo.FloodfillImage",  // Class identifier
    "Flood-fill Image",           // Display name
    "Undefined",                  // Category
    CodeState::Experimental,      // Code state
    Tags::None,                   // Tags
};
const ProcessorInfo FloodfillImage::getProcessorInfo() const { return processorInfo_; }

FloodfillImage::FloodfillImage()
    : Processor()
    , inImage_("inImage")
    , outImage_("IntSegmentation")
    , outVis_("ColorVis")
    , outMaxima_("maxima")
    , threshold_("threshold", "Threshold", 0.5, -10.0, 10.0, 0.1)
    , absolute_("absolute", "Absolute Values", true)
    , numClusters_("numClusters", "#Clusters", 42, 0, 100, 1, InvalidationLevel::Valid,
                   PropertySemantics::Text) {

    addPort(inImage_);
    addPort(outImage_);
    addPort(outMaxima_);
    addPort(outVis_);
    numClusters_.setReadOnly(true);
    addProperties(threshold_, absolute_, numClusters_);
}

void FloodfillImage::process() {
    if (!inImage_.hasData()) {
        outImage_.setData(std::shared_ptr<Image>(nullptr));
        outMaxima_.setData(nullptr);
        return;
    }
    LogWarn("Flood-filling!");
    const Layer* inLayer = inImage_.getData()->getColorLayer();
    size2_t gridDims = inLayer->getDimensions();

    std::shared_ptr<LayerRAMPrecision<int>> outLayer =
        std::make_shared<LayerRAMPrecision<int>>(gridDims);
    std::shared_ptr<LayerRAMPrecision<vec3>> visLayer =
        std::make_shared<LayerRAMPrecision<vec3>>(gridDims);
    int* outRaw = outLayer->getDataTyped();
    vec3* visRaw = visLayer->getDataTyped();
    auto inRAM = inLayer->getRepresentation<LayerRAM>();

    double threshold = threshold_;
    bool absolute = absolute_;

    auto maximaIndices = std::make_shared<std::vector<ivec2>>();
    std::vector<double> maximaValues;

    // Create queue.
    std::vector<ivec2> queue;
    static constexpr int BORDER = 2;
    for (unsigned y = BORDER; y < gridDims.y - BORDER; ++y)
        for (unsigned x = BORDER; x < gridDims.x - BORDER; ++x) {
            int linIdx = x + gridDims.x * y;
            ivec2 idx = {x, y};
            if (outRaw[linIdx] != 0) continue;

            // Below threshold: set output to -1.
            double value = inRAM->getAsDouble(idx);
            if (absolute) value = std::abs(value);
            if (value <= threshold) {
                outRaw[linIdx] = -1;
                continue;
            }
            LogWarn("New cluster!");
            // Above value? Start floodfill with new ID.
            maximaValues.push_back(value);
            maximaIndices->push_back(idx);
            int id = maximaValues.size();

            queue.push_back(idx);

            // Flood-fill.
            while (queue.size() > 0) {
                idx = queue.back();
                queue.pop_back();

                for (int dim = 0; dim < 2; ++dim)
                    for (int sign = -1; sign <= 1; sign += 2) {
                        ivec2 neighbor = idx;
                        neighbor[dim] += sign;

                        // Neighbor inside and not yet visited?
                        if (neighbor[dim] == -1 + BORDER ||
                            neighbor[dim] == static_cast<int>(gridDims[dim]) - BORDER)
                            continue;
                        linIdx = neighbor.x + neighbor.y * gridDims.x;
                        if (outRaw[linIdx] != 0) continue;

                        // Actually in the cluster?
                        value = inRAM->getAsDouble(neighbor);
                        if (absolute) value = std::abs(value);
                        if (value <= threshold) {
                            outRaw[linIdx] = -1;
                            continue;
                        }

                        if (value > maximaValues.back()) {
                            maximaValues.back() = value;
                            maximaIndices->back() = neighbor;
                        }
                        outRaw[linIdx] = id;
                        queue.push_back(neighbor);
                    }
            }
        }
    for (unsigned y = 0; y < gridDims.y; ++y)
        for (unsigned x = 0; x < gridDims.x; ++x) {
            int linIdx = x + gridDims.x * y;

            // TODO: REMOVE!
            visRaw[linIdx] =
                vec3(inRAM->getAsDouble({linIdx % gridDims.x, linIdx / gridDims.x}) * 10.0);
            // if (outRaw[linIdx] == 0) {
            //     visRaw[linIdx] = vec3(1.0);
            // } else if (outRaw[linIdx] == -1) {
            //     visRaw[linIdx] = vec3(0.5);
            // } else {
            if (outRaw[linIdx] > 0) visRaw[linIdx] = vec3(1, 0, 0);
            // vec3(0.5 + 0.5 * static_cast<float>(1.0 + outRaw[linIdx]) / maximaValues.size(), 0,
            // 0);
            // }
        }

    for (size_t i = 0; i < maximaIndices->size(); ++i) {
        ivec2 idx = maximaIndices->operator[](i);
        static constexpr int RAD = 5;
        for (int y = -RAD; y <= RAD; ++y)
            for (int x = -RAD; x <= RAD; ++x)
                visRaw[idx.x + x + (idx.y + y) * gridDims.x] = vec3(0, 1, 1);
    }
    outMaxima_.setData(maximaIndices);
    outImage_.setData(std::make_shared<Image>(std::make_shared<Layer>(outLayer)));
    outVis_.setData(std::make_shared<Image>(std::make_shared<Layer>(visLayer)));

    numClusters_.set(maximaValues.size(), 0, maximaValues.size());
}

}  // namespace inviwo
