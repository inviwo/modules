/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2026 Inviwo Foundation
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

#include <inviwo/devtools/processors/logrendererprocessors.h>
#include <modules/opengl/texture/textureutils.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/util/stringconversion.h>

#include <fmt/format.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo LogRendererProcessors::processorInfo_{
    "org.inviwo.LogRendererProcessors",  // Class identifier
    "Log Renderer Processors",           // Display name
    "Debugging",                         // Category
    CodeState::Experimental,             // Code state
    Tags::None,                          // Tags
    "A Processor that listens to Log messages and prints them to a canvas."_help};
const ProcessorInfo& LogRendererProcessors::getProcessorInfo() const { return processorInfo_; }

LogRendererProcessors::LogRendererProcessors()
    : Processor()
    , outport_("outport")
    , font_("font", "Font")
    , fmt_("format", "Format", "{6}")
    , fmtDesc_("fmtDesc", "Format Description",
               "Supports the formatting syntax used by fmt::format\n{0}: Source\n{1}: Level\n{2}: "
               "Audience\n{3}: File\n{4}: Function\n{5}: Line\n{6}: "
               "Message",
               InvalidationLevel::Valid, PropertySemantics::Multiline)
    , logInfo_("logInfo", "Log Info Messages", true)
    , logWarnings_("logWarnings", "Log Warnings", true)
    , logErrors_("logErrors", "Log Errors", true)
    , textRenderer_{}
    , loggerHandler_{std::make_shared<LoggerHandler>(this)} {
    fmtDesc_.setReadOnly(true);
    addPort(outport_);
    addProperties(fmt_, fmtDesc_, font_, logInfo_, logWarnings_, logErrors_);

    font_.anchorPos_.setVisible(false);

    LogCentral::getPtr()->registerLogger(loggerHandler_);
}

void LogRendererProcessors::process() {
    utilgl::activateAndClearTarget(outport_, ImageType::ColorDepth);
    utilgl::DepthFuncState depthFunc(GL_ALWAYS);
    utilgl::BlendModeState blending(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    vec2 dim(outport_.getDimensions());
    auto invDim = 2.0f / dim;

    textRenderer_.setFont(font_.getFontFace());
    textRenderer_.setFontSize(font_.getFontSize());
    textRenderer_.setLineSpacing(font_.getLineSpacing());

    auto maxRows = outport_.getDimensions().y / textRenderer_.getLineHeight();

    auto getText = [&]() -> std::string {
        if (rows_.size() > maxRows) {
            return joinString(rows_.end() - maxRows, rows_.end(), "\n");
        } else {
            return joinString(rows_, "\n");
        }
    };

    auto height = textRenderer_.computeBoundingBox(getText()).textExtent.y;
    auto y = (height) / (float)(dim.y);

    textRenderer_.render(getText(), vec2(-1, 2 * y - 1), invDim, vec4(1, 1, 1, 1));

    utilgl::deactivateCurrentTarget();
}

void LogRendererProcessors::LoggerHandler::log(std::string_view logSource, LogLevel logLevel,
                                               LogAudience audience, std::string_view file,
                                               std::string_view function, int line,
                                               std::string_view msg) {
    if (logLevel == LogLevel::Info && !owner_->logInfo_.get()) return;
    if (logLevel == LogLevel::Warn && !owner_->logWarnings_.get()) return;
    if (logLevel == LogLevel::Error && !owner_->logErrors_.get()) return;
    msg = fmt::format(fmt::runtime(owner_->fmt_.get()), logSource, logLevel, audience, file,
                      function, line, msg);
    owner_->rows_.emplace_back(msg);

    dispatchFront([network = owner_->getNetwork(), id = owner_->getIdentifier()]() {
        auto p = network->getProcessorByIdentifier(id);
        if (p) p->invalidate(InvalidationLevel::InvalidOutput);
    });
}

}  // namespace inviwo
