/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2020 Inviwo Foundation
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

#include <inviwo/tensorvisbase/processors/tensorglyphprocessor.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorGlyphProcessor::processorInfo_{
    "org.inviwo.TensorGlyphProcessor",  // Class identifier
    "Tensor Glyphs",                    // Display name
    "Tensor visualization",             // Category
    CodeState::Stable,                  // Code state
    tag::OpenTensorVis | Tag::CPU,      // Tags
};

const ProcessorInfo TensorGlyphProcessor::getProcessorInfo() const { return processorInfo_; }

TensorGlyphProcessor::TensorGlyphProcessor()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , glyphParameters_("glyphParameters", "Glyph parameters")

{
    addPort(outport_);
    addPort(inport_);

    addProperty(glyphParameters_);
}

void TensorGlyphProcessor::process() {
    if (!inport_.hasData() || !inport_.getData().get()) return;

    auto tensorField = inport_.getData();

    auto dimensions = tensorField->getDimensions();

    util::IndexMapper3D indexMapper(dimensions);

    auto comp = glm::zero<mat3>();

    auto meshes = new std::vector<std::shared_ptr<Mesh>>();

    const vec3 voxelDist{tensorField->getSpacing()};
    const vec3 offset{tensorField->getOffset()};

    for (size_t z = 0; z < dimensions.z; z++) {
        for (size_t y = 0; y < dimensions.y; y++) {
            for (size_t x = 0; x < dimensions.x; x++) {
                const auto& tensor = tensorField->at(size3_t(x, y, z));
                if (tensor == comp) continue;
                const auto index = indexMapper(size3_t(x, y, z));

                // const auto dpos = tensorField->getNormalizedVolumePosition(index);
                // const auto pos = vec3(dpos);

                const vec3 pos{voxelDist * vec3{x, y, z} + offset};

                meshes->emplace_back(glyphParameters_.generateGlyph(tensorField, index, pos));
            }
        }
    }

    outport_.setData(meshes);
}
}  // namespace inviwo
