/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017 Inviwo Foundation
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

#pragma optimize("",off)

#include <inviwo/heightfieldrendering/processors/imagetopoints.h>
#include <inviwo/core/datastructures/image/layerram.h>
#include <inviwo/core/datastructures/image/imageram.h>
#include <inviwo/core/util/indexmapper.h>
#include <algorithm>

namespace inviwo {

    // The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
    const ProcessorInfo ImageToPoints::processorInfo_{
        "org.inviwo.ImageToPoints",  // Class identifier
        "Image To Points",           // Display name
        "Mesh Creation",             // Category
        CodeState::Experimental,     // Code state
        Tags::None,                  // Tags
    };
    const ProcessorInfo ImageToPoints::getProcessorInfo() const { return processorInfo_; }

    ImageToPoints::ImageToPoints()
        : Processor()
        , inport_("inport",true)
        , outport_("outport")
        , transferFunctionProperty_("transferFunctionProperty", "Transfer function")
        , heightScale_("heightScale", "Height scale", 1.f, 0.f, 2.f, 0.01f) {
        addPort(inport_);
        addPort(outport_);
        addProperty(transferFunctionProperty_);
        addProperty(heightScale_);
    }

    void ImageToPoints::process() {
        const auto image = inport_.getData();

        const auto dimensions = image->getDimensions();

        const auto numSpheres = dimensions.x * dimensions.y;
        const auto imageData =
            static_cast<const float*>(image->getColorLayer()->getRepresentation<LayerRAM>()->getData());
        const auto imageRAM = image->getRepresentation<ImageRAM>()->getColorLayerRAM();

        auto max = std::numeric_limits<float>::lowest();
        for (size_t i = 0; i < numSpheres; i++) {
            max = std::max(max, imageData[i]);
        }

        util::IndexMapper2D indexMapper(dimensions);
        const auto xFrac = 1.f / static_cast<float>(dimensions.x - 1);
        const auto yFrac = 1.f / static_cast<float>(dimensions.y - 1);
        const auto& tf = transferFunctionProperty_.get();

        auto mesh = std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);

        auto vertexRAM = std::make_shared<BufferRAMPrecision<vec3>>(numSpheres);
        auto colorRAM = std::make_shared<BufferRAMPrecision<vec4>>(numSpheres);
        auto radiiRAM = std::make_shared<BufferRAMPrecision<float>>(numSpheres);

        mesh->addBuffer(Mesh::BufferInfo(BufferType::PositionAttrib),
            std::make_shared<Buffer<vec3>>(vertexRAM));
        mesh->addBuffer(Mesh::BufferInfo(BufferType::ColorAttrib),
            std::make_shared<Buffer<vec4>>(colorRAM));
        mesh->addBuffer(Mesh::BufferInfo(BufferType::NumberOfBufferTypes, 5),
            std::make_shared<Buffer<float>>(radiiRAM));

        auto& vertices = vertexRAM->getDataContainer();
        auto& colors = colorRAM->getDataContainer();
        auto& radii = radiiRAM->getDataContainer();

        for (size_t x = 0; x < dimensions.x; x++) {
            for (size_t y = 0; y < dimensions.y; y++) {
                const auto pos = indexMapper(x, y);

                const auto val = imageRAM->getAsDouble(size2_t(x, y));
                const auto color = tf.sample(val / max);

                vertices[pos] = vec3(x * xFrac, y * yFrac, val * heightScale_.get());
                colors[pos] = color;
                radii[pos] = 1.f / static_cast<float>(dimensions.x - 1);
            }
        }
        outport_.setData(mesh);
    }

}  // namespace inviwo
#pragma optimize("",on)