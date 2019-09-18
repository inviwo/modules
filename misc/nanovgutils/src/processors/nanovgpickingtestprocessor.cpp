/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2019 Inviwo Foundation
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

#include <inviwo/nanovgutils/nanovgutilsmodule.h>
#include <inviwo/nanovgutils/processors/nanovgpickingtestprocessor.h>
#include <modules/opengl/texture/textureutils.h>
#include <inviwo/core/interaction/events/mouseevent.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo NanoVGPickingTestProcessor::processorInfo_{
    "org.inviwo.NanoVGPickingTestProcessor",  // Class identifier
    "NanoVG Picking Test Processor",          // Display name
    "Testing",                                // Category
    CodeState::Experimental,                  // Code state
    Tags::None,                               // Tags
};
const ProcessorInfo NanoVGPickingTestProcessor::getProcessorInfo() const { return processorInfo_; }

NanoVGPickingTestProcessor::NanoVGPickingTestProcessor()
    : Processor()
    , outport_("outport")
    , eventPorperty_("posiiotn", "Position",
                     [this](Event* e) {
                         if (auto ev = e->getAs<MouseEvent>()) {
                             mousePos_ = ev->pos();
							 invalidate(InvalidationLevel::InvalidOutput);
                         }
                     },
                     MouseButton::None, MouseState::Move)
    , nvgContext_(
          InviwoApplication::getPtr()->getModuleByType<NanoVGUtilsModule>()->getNanoVGContext()) {

    addPort(outport_);

	addProperty(eventPorperty_);
}

void NanoVGPickingTestProcessor::process() {
    auto dimensions = outport_.getDimensions();

    utilgl::activateAndClearTarget(outport_, ImageType::ColorDepth);

    nvgContext_->activate(dimensions);

    nvgContext_->beginPath();

    nvgContext_->circle(vec2(dimensions / size2_t(2)), 100.f);

	nvgContext_->fillColor(vec4(1.f));
	nvgContext_->fillHitRegion(5);
	nvgContext_->fill();

    auto id = nvgContext_->hitTest(mousePos_.x, mousePos_.y, NanoVGContext::HitTestRegion::All);

    nvgContext_->deactivate();

    utilgl::deactivateCurrentTarget();

    LogInfo("ID hit: " << std::to_string(id));
}

}  // namespace inviwo
