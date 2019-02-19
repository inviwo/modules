/*********************************************************************************
*
* Inviwo - Interactive Visualization Workshop
*
* Copyright (c) 2016-2018 Inviwo Foundation
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

#include <modules/tensorvisio/processors/vtktotensorfield3d.h>
#include <fstream>
#include <ios>

namespace inviwo {
// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKToTensorField3D::processorInfo_{
    "org.inviwo.VTKToTensorField3D",  // Class identifier
    "VTK to 3D tensor field",         // Display name
    "Tensor Visualization",           // Category
    CodeState::Experimental,          // Code state
    Tags::CPU,                        // Tags
};

const ProcessorInfo VTKToTensorField3D::getProcessorInfo() const { return processorInfo_; }

VTKToTensorField3D::VTKToTensorField3D()
    : Processor()
    , inport_("inport")
    , outport_("outport") {

    addPort(inport_);
    addPort(outport_);
}

void VTKToTensorField3D::process() {
    const auto dimensions = inport_.getData()->first;
    const auto ddimensions = dvec3(dimensions);
    const auto rawTensorData = inport_.getData()->second;

    const auto max = std::max(ddimensions.x, std::max(ddimensions.y, ddimensions.z));
    const auto extends = dvec3(ddimensions.x / max, ddimensions.y / max, ddimensions.z / max);

    outport_.setData(std::make_shared<TensorField3D>(dimensions, rawTensorData, extends));
}
}