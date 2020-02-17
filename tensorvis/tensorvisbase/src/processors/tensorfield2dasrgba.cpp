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

#include <inviwo/tensorvisbase/processors/tensorfield2dasrgba.h>
#include <inviwo/tensorvisbase/algorithm/tensorfieldsampling.h>
#include <inviwo/core/util/imagesampler.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField2DAsRGBA::processorInfo_{
    "org.inviwo.TensorField2DAsRGBA",  // Class identifier
    "Tensor Field 2D As RGBA",         // Display name
    "Tensor visualization",            // Category
    CodeState::Experimental,           // Code state
    tag::OpenTensorVis | Tag::GL,      // Tags
};

const ProcessorInfo TensorField2DAsRGBA::getProcessorInfo() const { return processorInfo_; }

TensorField2DAsRGBA::TensorField2DAsRGBA()
    : Processor()
    , inport_("inport")
    , outport_("outport", DataVec4Float32::get())
    , shader_("tensorfieldtorgba.frag")
    , hover_("hover", "Hover", [this](Event* e) { hoverAction(e); }, MouseButtons(flags::any),
             MouseState::Move)
    , tensor_("tensor", "Tensor", glm::mat<2, 2, TensorField2D::value_type>{0},
              glm::mat<2, 2, TensorField2D::value_type>{-10000},
              glm::mat<2, 2, TensorField2D::value_type>{10000},
              glm::mat<2, 2, TensorField2D::value_type>{0.00001f}) {
    shader_.onReload([&]() { invalidate(InvalidationLevel::InvalidOutput); });

    addPort(inport_);
    addPort(outport_);

    addProperty(hover_);
    addProperty(tensor_);
}

void TensorField2DAsRGBA::process() {
    utilgl::activateAndClearTarget(outport_);

    shader_.activate();
    TextureUnitContainer units;

    utilgl::bindAndSetUniforms(shader_, units, *inport_.getData(), "tensorField",
                               ImageType::ColorOnly);

    utilgl::singleDrawImagePlaneRect();
    shader_.deactivate();
    utilgl::deactivateCurrentTarget();
}

void TensorField2DAsRGBA::hoverAction(Event* e) {
    using m_type = glm::mat<2, 2, TensorField2D::value_type>;
    using v_type = glm::vec<2, TensorField2D::value_type>;

    if (inport_.hasData() && inport_.getData().get()) {
        if (auto mouseEvent = dynamic_cast<MouseEvent*>(e)) {
            auto tensorField = inport_.getData();
            auto p = mouseEvent->posNormalized();

            ImageSpatialSampler<4, TensorField2D::value_type> sampler(inport_.getData());
            auto vec = sampler.sample(p.x, p.y);

            tensor_.set(m_type(v_type(vec[0], vec[1]), v_type(vec[2], vec[3])));
        }
    }
}

}  // namespace inviwo
