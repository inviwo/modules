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

#include <inviwo/tensorvisbase/processors/tensorfieldtorgba.h>
#include <inviwo/tensorvisbase/algorithm/tensorfieldsampling.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorFieldToRGBA::processorInfo_{
    "org.inviwo.Tensor Field To RGBA",  // Class identifier
    "TensorFieldToRGBA",                // Display name
    "Tensor visualization",             // Category
    CodeState::Experimental,            // Code state
    Tags::GL,                           // Tags
};

const ProcessorInfo TensorFieldToRGBA::getProcessorInfo() const { return processorInfo_; }

TensorFieldToRGBA::TensorFieldToRGBA()
    : Processor()
    , inport_("inport")
    , outport_("outport", DataVec4Float32::get())
    , shader_("tensorfieldtorgba.frag")
    , hover_(
          "hover", "Hover", [this](Event* e) { hoverAction(e); }, MouseButtons(flags::any),
          MouseState::Move)
    , tensor_("tensor", "Tensor") {
    shader_.onReload([&]() { invalidate(InvalidationLevel::InvalidOutput); });

    addPort(inport_);
    addPort(outport_);

    addProperty(hover_);
    addProperty(tensor_);
}

void TensorFieldToRGBA::process() {
    utilgl::activateAndClearTarget(outport_);

    shader_.activate();
    TextureUnitContainer units;
    std::shared_ptr<Image> tensorFieldTexture;
    tensorutil::bindTensorFieldAsColorTexture(tensorFieldTexture, inport_.getData(), shader_,
                                              units);

    utilgl::singleDrawImagePlaneRect();
    shader_.deactivate();
    utilgl::deactivateCurrentTarget();
}

void TensorFieldToRGBA::hoverAction(Event* e) {
    if (inport_.hasData() && inport_.getData().get()) {
        if (auto mouseEvent = dynamic_cast<MouseEvent*>(e)) {
            auto tensorField = inport_.getData();
            auto p = mouseEvent->posNormalized();
			auto tensor = sample(tensorField, p);
            tensor_.set(tensor);
        }
    }
}

}  // namespace inviwo
