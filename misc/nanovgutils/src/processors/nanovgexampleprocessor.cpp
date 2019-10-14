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

#include <inviwo/nanovgutils/processors/nanovgexampleprocessor.h>
#include <inviwo/nanovgutils/nanovgutils.h>
#include <modules/opengl/texture/textureutils.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo NanoVGExampleProcessor::processorInfo_{
    "org.inviwo.NanoVGExampleProcessor",  // Class identifier
    "NanoVG Example",                     // Display name
    "Example",                            // Category
    CodeState::Experimental,              // Code state
    Tags::None,                           // Tags
};
const ProcessorInfo NanoVGExampleProcessor::getProcessorInfo() const { return processorInfo_; }

NanoVGExampleProcessor::NanoVGExampleProcessor() : Processor(), outport_("outport") {

    addPort(outport_);
}

void NanoVGExampleProcessor::process() {

    auto& nvg = nanovgutil::getContext();

    utilgl::activateAndClearTarget(outport_, ImageType::ColorOnly);

    // Activate NVG for drawing using "pixel coordinates" i.e. Coordinates used for defining shapes
    // are in the range of [0 canvassize]
    nvg.activate(outport_.getDimensions());

    // Draw a circle with of raidus 50 and x-y coordinates 150,150
    nvg.arc(vec2(150, 150), 50, 0, glm::two_pi<float>(), NVG_CW);

    // Set fill color to red
    nvg.fillColor(vec4(1, 0, 0, 1));
    nvg.fill();  // tell NVG to draw current shape

    nvg.deactivate();
    utilgl::deactivateCurrentTarget();
}

}  // namespace inviwo
