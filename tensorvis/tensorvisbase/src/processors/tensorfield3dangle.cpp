﻿/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2025 Inviwo Foundation
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

#include <inviwo/tensorvisbase/processors/tensorfield3dangle.h>
#include <glm/gtx/vector_angle.hpp>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField3DAngle::processorInfo_{
    "org.inviwo.TensorField3DAngle",  // Class identifier
    "Tensor Field 3D Angle",          // Display name
    "Tensor",                         // Category
    CodeState::Experimental,          // Code state
    Tags::CPU,                        // Tags
};
const ProcessorInfo& TensorField3DAngle::getProcessorInfo() const { return processorInfo_; }

TensorField3DAngle::TensorField3DAngle()
    : Processor()
    , tensorField3DInport1_("tensorField3DInport1")
    , fiberOrientation_("fiberOrientation")
    , invariantSpaceOutport_("invariantSpaceOutport") {
    addPort(tensorField3DInport1_);
    addPort(fiberOrientation_);
    addPort(invariantSpaceOutport_);
}

void TensorField3DAngle::process() {
    if (!tensorField3DInport1_.hasData() || !fiberOrientation_.hasData()) return;

    auto tf1 = tensorField3DInport1_.getData();
    auto fot = fiberOrientation_.getData();

    const auto& maev1 = tf1->majorEigenVectors();
    const auto& inev1 = tf1->middleEigenVectors();
    const auto& miev1 = tf1->minorEigenVectors();

    const auto& maev2 = fot->majorEigenVectors();

    const auto numberOfElements = maev1.size();

    auto iv = std::make_shared<InvariantSpace>(
        3, std::vector<std::string>{"φmax", "φmiddle", "φmin"},
        std::vector<TensorFeature>{TensorFeature::Unspecified, TensorFeature::Unspecified,
                                   TensorFeature::Unspecified});

    auto convertAngle = [](auto angle) {
        return angle >= glm::half_pi<double>() ? glm::pi<double>() - angle : angle;
    };

    for (size_t i = 0; i < numberOfElements; ++i) {
        iv->addPoint({convertAngle(glm::angle(maev1[i], maev2[i])),
                      convertAngle(glm::angle(inev1[i], maev2[i])),
                      convertAngle(glm::angle(miev1[i], maev2[i]))});
    }

    invariantSpaceOutport_.setData(iv);
}

}  // namespace inviwo
