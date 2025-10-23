/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2021 Inviwo Foundation
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

#include <inviwo/computeutils/processors/volumechannelsplitglprocessor.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VolumeChannelSplitGLProcessor::processorInfo_{
    "org.inviwo.VolumeChannelSplitGLProcessor",  // Class identifier
    "Volume Channel Split Processor",            // Display name
    "Volume Operation",                          // Category
    CodeState::Experimental,                     // Code state
    Tags::GL,                                    // Tags
};
const ProcessorInfo VolumeChannelSplitGLProcessor::getProcessorInfo() const {
    return processorInfo_;
}

VolumeChannelSplitGLProcessor::VolumeChannelSplitGLProcessor()
    : Processor(), volumeInport_("volumeInport"), volumeOutport_("volumeOutport") {

    addPorts(volumeInport_, volumeOutport_);
}

void VolumeChannelSplitGLProcessor::process() {
    if (!volumeInport_.hasData() || !volumeInport_.getData()) {
        return;
    }

    volumeOutport_.setData(volumeChannelSplitGl_.split(volumeInport_.getData()));
}

}  // namespace inviwo
