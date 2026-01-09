/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2026 Inviwo Foundation
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

#include <inviwo/nanovgutils/processors/nanovgpickingexampleprocessor.h>
#include <inviwo/nanovgutils/nanovgutils.h>
#include <modules/opengl/texture/textureutils.h>
#include <inviwo/core/interaction/pickingstate.h>
#include <inviwo/core/interaction/events/pickingevent.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo NanoVGPickingExampleProcessor::processorInfo_{
    "org.inviwo.NanoVGPickingExampleProcessor",  // Class identifier
    "NanoVG Picking Example",                    // Display name
    "Example",                                   // Category
    CodeState::Experimental,                     // Code state
    Tags::None,                                  // Tags
};
const ProcessorInfo& NanoVGPickingExampleProcessor::getProcessorInfo() const {
    return processorInfo_;
}

NanoVGPickingExampleProcessor::NanoVGPickingExampleProcessor()
    : Processor()
    , outport_("outport")
    , picking_(this, 4, [&](PickingEvent* pe) { onItemPicked(pe); }) {

    addPort(outport_);
}

void NanoVGPickingExampleProcessor::process() {
    utilgl::activateAndClearTarget(outport_, ImageType::ColorOnly);
    draw(false);
    glDrawBuffer(GL_COLOR_ATTACHMENT7);  // Tell GL to render to the Picking Attachment of the FBO
    draw(true);
    utilgl::deactivateCurrentTarget();
}

void NanoVGPickingExampleProcessor::draw(bool picking) {
    const static std::array<std::tuple<vec2, float, vec4>, 4> objects = {
        std::make_tuple(vec2{50, 50}, 40.f, vec4(1, 0, 0, 1)),
        std::make_tuple(vec2{250, 50}, 20.f, vec4(0, 1, 0, 1)),
        std::make_tuple(vec2{50, 250}, 30.f, vec4(0, 0, 1, 1)),
        std::make_tuple(vec2{200, 220}, 75.f, vec4(1, 0, 1, 1))};

    auto& nvg = nanovgutil::getContext();
    nvg.activate(outport_.getDimensions());
    if (picking) {
        nvg.shapeAntiAlias(false);
    }

    size_t i = 0;
    for (auto [pos, r, color] : objects) {
        auto id = i++;
        nvg.beginPath();
        nvg.circle(pos, r);
        if (picking) {
            nvg.fillColor(vec4(picking_.getColor(id), 1.0f));
        } else {
            nvg.fillColor(color);
        }
        nvg.fill();
        nvg.closePath();
    }
    if (picking) {
        nvg.shapeAntiAlias(true);
    }
    nvg.deactivate();
}

void NanoVGPickingExampleProcessor::onItemPicked(PickingEvent* p) {
    if (p->getState() == PickingState::Updated && p->getPressState() == PickingPressState::Press &&
        p->getPressItem() == PickingPressItem::Primary) {
        auto [_, id] = p->getCurrentLocalPickingId();
        LogInfo("Item " << id << " picked");
    }
}

}  // namespace inviwo
