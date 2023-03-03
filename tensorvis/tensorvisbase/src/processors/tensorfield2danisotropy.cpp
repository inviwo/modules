/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2023 Inviwo Foundation
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

#include <inviwo/tensorvisbase/processors/tensorfield2danisotropy.h>
#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/datastructures/image/imageram.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/tensorvisbase/util/tensorfieldutil.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField2DAnisotropy::processorInfo_{
    "org.inviwo.TensorField2DAnisotropy",  // Class identifier
    "Tensor Field 2D Anisotropy",          // Display name
    "Tensor visualization",                // Category
    CodeState::Experimental,               // Code state
    Tags::None,                            // Tags
};
const ProcessorInfo TensorField2DAnisotropy::getProcessorInfo() const { return processorInfo_; }

TensorField2DAnisotropy::TensorField2DAnisotropy()
    : Processor()
    , tensorFieldInport_("inport")
    , outport_("outport", false)
    , anisotropy_("anisotropy", "Anisotropy",
                  {{"1", "|" + tensorutil::lamda1_str + " - " + tensorutil::lamda2_str + "|",
                    tensorutil::Anisotropy::abs_lamda1_minus_lamda2},
                   {"4", "|" + tensorutil::lamda1_str + "| - |" + tensorutil::lamda2_str + "|",
                    tensorutil::Anisotropy::abs_lamda1_minus_abs_lamda2}},
                  0) {

    addPort(tensorFieldInport_);
    addPort(outport_);
    addProperty(anisotropy_);
}

void TensorField2DAnisotropy::process() {
    auto tensorField = tensorFieldInport_.getData();
    std::shared_ptr<Image> outputImage;
    auto dimensions = tensorField->getDimensions();
    std::function<void(const size2_t&)> func;
    util::IndexMapper2D indexMapper(dimensions);
    const auto& majorEigenValues = tensorField->majorEigenValues();
    const auto& minorEigenValues = tensorField->minorEigenValues();
    auto min = std::numeric_limits<double>::max();
    auto max = std::numeric_limits<double>::lowest();

    switch (anisotropy_.get()) {
        case tensorutil::Anisotropy::abs_lamda1_minus_lamda2:
            outputImage = std::make_shared<Image>(dimensions, DataFloat32::get());
            {
                auto data =
                    static_cast<glm::f32*>(outputImage->getEditableRepresentation<ImageRAM>()
                                               ->getColorLayerRAM()
                                               ->getData());

                tensorutil::forEachFixelParallel(
                    *(tensorField.get()), [&](const size2_t& pos) -> void {
                        const auto index = indexMapper(pos);
                        const auto val =
                            glm::abs(majorEigenValues[index] - minorEigenValues[index]);
                        max = std::max(max, val);
                        min = std::min(min, val);
                        data[index] = static_cast<glm::f32>(val);
                    });
            }
            break;
        case tensorutil::Anisotropy::abs_lamda1_minus_abs_lamda2:
            outputImage = std::make_shared<Image>(dimensions, DataFloat32::get());
            {
                auto data =
                    static_cast<glm::f32*>(outputImage->getEditableRepresentation<ImageRAM>()
                                               ->getColorLayerRAM()
                                               ->getData());

                tensorutil::forEachFixelParallel(
                    *(tensorField.get()), [&](const size2_t& pos) -> void {
                        const auto index = indexMapper(pos);
                        const auto val =
                            glm::abs(majorEigenValues[index]) - glm::abs(minorEigenValues[index]);

                        max = std::max(max, val);
                        min = std::min(min, val);
                        data[index] = static_cast<glm::f32>(val);
                    });
            }
            break;
        default:
            break;
    }

    outport_.setData(outputImage);
}

}  // namespace inviwo
