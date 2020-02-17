/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2020 Inviwo Foundation
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

#include <inviwo/tensorvisbase/processors/tensorfield3dfiberangle.h>
#include <glm/gtx/vector_angle.hpp>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/buffer/bufferramprecision.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField3DFiberAngle::processorInfo_{
    "org.inviwo.TensorField3DFiberAngle",  // Class identifier
    "Tensor Field 3D Fiber Angle",         // Display name
    "OpenTensorVis",                       // Category
    CodeState::Experimental,               // Code state
    tag::OpenTensorVis | Tag::CPU,         // Tags
};
const ProcessorInfo TensorField3DFiberAngle::getProcessorInfo() const { return processorInfo_; }

TensorField3DFiberAngle::TensorField3DFiberAngle()
    : Processor()
    , stressField_("stressField")
    , fiberField_("fiberField")
    , angleOutport_("angleOutport") {
    addPort(stressField_);
    addPort(fiberField_);
    addPort(angleOutport_);
}

void TensorField3DFiberAngle::process() {
    if (!stressField_.hasData() || !fiberField_.hasData()) return;

    auto stressField = stressField_.getData();
    auto fiberField = fiberField_.getData();

    const auto& maev1 = stressField->majorEigenVectors();
    const auto& inev1 = stressField->intermediateEigenVectors();
    const auto& miev1 = stressField->minorEigenVectors();

    const auto& maev2 = fiberField->majorEigenVectors();

    const auto numberOfElements = maev1.size();

    auto dataFrame = std::make_shared<DataFrame>();

    auto phiMajor = std::make_shared<BufferRAMPrecision<float>>(maev1.size());
    auto phiIntermediate = std::make_shared<BufferRAMPrecision<float>>(inev1.size());
    auto phiMinor = std::make_shared<BufferRAMPrecision<float>>(miev1.size());

    auto convertAngle = [](const auto angle) -> float {
        return angle >= glm::half_pi<float>() ? glm::pi<float>() - angle : angle;
    };

    for (size_t i = 0; i < numberOfElements; ++i) {
        phiMajor->operator[](i) = convertAngle(glm::angle(maev1[i], maev2[i]));
        phiIntermediate->operator[](i) = convertAngle(glm::angle(inev1[i], maev2[i]));
        phiMinor->operator[](i) = convertAngle(glm::angle(miev1[i], maev2[i]));
    }

    dataFrame->addColumnFromBuffer(u8"φmax", std::make_shared<Buffer<float>>(phiMajor));
    dataFrame->addColumnFromBuffer(u8"φintermediate",
                                   std::make_shared<Buffer<float>>(phiIntermediate));
    dataFrame->addColumnFromBuffer(u8"φmin", std::make_shared<Buffer<float>>(phiMinor));

    angleOutport_.setData(dataFrame);
}

}  // namespace inviwo
