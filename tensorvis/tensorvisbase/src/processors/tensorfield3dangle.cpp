/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019 Inviwo Foundation
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

#include <modules/tensorvisbase/processors/tensorfield3dangle.h>
#include <glm/gtx/vector_angle.hpp>
#include <math.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField3DAngle::processorInfo_{
    "org.inviwo.TensorField3DAngle",  // Class identifier
    "Tensor Field 3D Angle",          // Display name
    "Tensor",                         // Category
    CodeState::Experimental,          // Code state
    Tags::CPU,                        // Tags
};
const ProcessorInfo TensorField3DAngle::getProcessorInfo() const { return processorInfo_; }

TensorField3DAngle::TensorField3DAngle()
    : Processor()
    , tensorField3DInport1_("tensorField3DInport1")
    , tensorField3DInport2_("tensorField3DInport2")
    , invariantSpaceOutport_("invariantSpaceOutport") {
    addPort(tensorField3DInport1_);
    addPort(tensorField3DInport2_);
    addPort(invariantSpaceOutport_);
}

void TensorField3DAngle::process() {
    if (!tensorField3DInport1_.hasData() || !tensorField3DInport2_.hasData()) return;

    auto tf1 = tensorField3DInport1_.getData();
    auto tf2 = tensorField3DInport2_.getData();

    const auto& stress_major = tf1->majorEigenVectors();
    const auto& stress_intermediate = tf1->middleEigenVectors();
    const auto& stress_minor = tf1->minorEigenVectors();

    const auto& fiber_major = tf2->majorEigenVectors();

    const auto numberOfElements = fiber_major.size();

    auto iv = std::make_shared<InvariantSpace>(
        3, std::vector<std::string>{"phi1", "phi2", "phi3"},
        std::vector<TensorFeature>{TensorFeature::Unspecified, TensorFeature::Unspecified,
                                   TensorFeature::Unspecified});

    for (size_t i = 0; i < numberOfElements; ++i) {
        auto angle1 = glm::angle(stress_major[i], fiber_major[i]);
        if (angle1 > M_PI / 2.0) angle1 = M_PI - angle1;

        auto angle2 = glm::angle(stress_intermediate[i], fiber_major[i]);
        if (angle2 > M_PI / 2.0) angle2 = M_PI - angle2;

        auto angle3 = glm::angle(stress_minor[i], fiber_major[i]);
        if (angle3 > M_PI / 2.0) angle3 = M_PI - angle3;

        iv->addPoint({angle1, angle2, angle3});
    }

    invariantSpaceOutport_.setData(iv);
}

}  // namespace inviwo
