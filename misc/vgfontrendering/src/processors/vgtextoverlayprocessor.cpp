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

#include <inviwo/vgfontrendering/processors/vgtextoverlayprocessor.h>
#include <inviwo/nanovgutils/nanovgutilsmodule.h>
#include <inviwo/vgfontrendering/vgfontrenderingmodule.h>

namespace inviwo {

const ProcessorInfo VGTextOverlayProcessor::processorInfo_{
    "org.inviwo.VGTextOverlay",  // Class identifier
    "VG text overlay",           // Display name
    "Fontrendering",             // Category
    CodeState::Experimental,     // Code state
    Tags::GL,                    // Tags
};

const ProcessorInfo VGTextOverlayProcessor::getProcessorInfo() const { return processorInfo_; }

VGTextOverlayProcessor::VGTextOverlayProcessor()
    : Processor()
    , inport_("inport")
    , outport_("ourport")
    , text_("text", "Text", "random string", InvalidationLevel::InvalidOutput,
            PropertySemantics::TextEditor)
    , file_("file", "Font file")
    , position_("position", "Position", ivec2(0, 0), ivec2(0, 0), ivec2(0, 0))
    , size_("size", "Size", 18.f, 1.f, 100.f, 0.1f)
    , textBoxWidth_("width", "Textbox width", 50.f, 10.f, 1000.f, 1.f)
    , color_("colour", "Color", vec4(0.f, 0.f, 0.f, 1.f))
    , numFontsLoaded_(-1)
    , horizontalAlignment_("halign", "Text alignment")
    , font_("font", "Font")
    , nvgContext_(
          InviwoApplication::getPtr()->getModuleByType<NanoVGUtilsModule>()->getNanoVGContext()) {
    // Ports
    inport_.setOptional(true);
    addPort(inport_);
    addPort(outport_);

    // Callbacks
    text_.onChange([&]() { invalidate(InvalidationLevel::InvalidOutput); });
    file_.onChange([&]() { invalidate(InvalidationLevel::InvalidResources); });

    // Properties setup
    color_.setSemantics(PropertySemantics::Color);
    horizontalAlignment_.addOption("left", "Left", 0);
    horizontalAlignment_.addOption("center", "Center", 1);
    horizontalAlignment_.addOption("right", "Right", 2);
    horizontalAlignment_.setSelectedValue(0);

    font_.addOption("light", "Roboto-Light", 0);
    font_.addOption("regular", "Roboto-Regular", 1);
    font_.addOption("bold", "Roboto-Bold", 2);
    font_.setSelectedValue(1);
    font_.setCurrentStateAsDefault();
    addProperty(font_);

    // Add properties
    addProperty(file_);
    addProperty(text_);
    addProperty(size_);
    addProperty(textBoxWidth_);
    addProperty(position_);
    addProperty(color_);
    // addProperty(verticalAlignment_);
    addProperty(horizontalAlignment_);

    auto path = InviwoApplication::getPtr()->getModuleByType<VGFontRenderingModule>()->getPath() +
                "/fonts/";

    if (nvgContext_->createFont("font" + ++numFontsLoaded_, path + "Roboto-Light.ttf") != -1 &&
        nvgContext_->createFont("font" + ++numFontsLoaded_, path + "Roboto-Regular.ttf") != -1 &&
        nvgContext_->createFont("font" + ++numFontsLoaded_, path + "Roboto-Bold.ttf") != -1) {
    } else {
        LogError("Failed to load fonts.");
    }
}

VGTextOverlayProcessor::~VGTextOverlayProcessor() {}

void VGTextOverlayProcessor::initializeResources() {
    if (outport_.isConnected()) {
        position_.setMaxValue(outport_.getDimensions());
    }
}

void VGTextOverlayProcessor::process() {
    if (!numFontsLoaded_) {
        LogError("You need to load a font!");
        return;
    }

    ivec2 dim = outport_.getDimensions();

    if (position_.getMaxValue() != dim && outport_.isConnected()) {
        position_.setMaxValue(dim);
    }

    utilgl::ClearColor colorstate(vec4(1, 1, 1, 1));

    if (inport_.isConnected()) {
        utilgl::activateTargetAndCopySource(outport_, inport_, ImageType::ColorDepth);
    } else {
        utilgl::activateAndClearTarget(outport_, ImageType::ColorDepth);
    }

    nvgContext_->activate(dim);

    // See nanovg.h for reference
    auto align = (1 << horizontalAlignment_.getSelectedValue()) | (1 << 3);

    nvgContext_->beginPath();
    nvgContext_->fontSize(size_);
    nvgContext_->fontFace("font" + std::to_string(font_.getSelectedValue()));
    nvgContext_->textAlign(align);
    nvgContext_->fillColor(color_);

    auto bounds = nvgContext_->textBoxBounds(position_, textBoxWidth_, text_);

    setAbsoluteAnchorPositionBounds(bounds);

    nvgContext_->textBox(position_, textBoxWidth_, text_);

    nvgContext_->closePath();

    nvgContext_->deactivate();

    utilgl::deactivateCurrentTarget();
}

void VGTextOverlayProcessor::setAbsoluteAnchorPositionBounds(vec4 textBoxBounds) {
    auto textBoxWidth = textBoxBounds.z - textBoxBounds.x;
    auto textBoxHeight = textBoxBounds.w - textBoxBounds.y;

    ivec2 dim = outport_.getDimensions();
    vec4 absoluteAnchorPositionBounds;

    absoluteAnchorPositionBounds.x = 0.0;
    absoluteAnchorPositionBounds.z = dim.x - textBoxWidth;
    absoluteAnchorPositionBounds.y = 0.0;
    absoluteAnchorPositionBounds.w = dim.y - textBoxHeight;

    auto position = position_.get();

    if (position.x < absoluteAnchorPositionBounds.x) {
        position.x = static_cast<int>(absoluteAnchorPositionBounds.x);
    }
    if (position.y < absoluteAnchorPositionBounds.y) {
        position.y = static_cast<int>(absoluteAnchorPositionBounds.y);
    }
    if (position.x > absoluteAnchorPositionBounds.z) {
        position.x = static_cast<int>(absoluteAnchorPositionBounds.z);
    }
    if (position.y > absoluteAnchorPositionBounds.w) {
        position.y = static_cast<int>(absoluteAnchorPositionBounds.w);
    }

    position_.setMinValue(vec2(absoluteAnchorPositionBounds));
    position_.setMaxValue(vec2(absoluteAnchorPositionBounds.z, absoluteAnchorPositionBounds.w));
}

}  // namespace inviwo