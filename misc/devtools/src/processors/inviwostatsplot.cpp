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

#include <inviwo/devtools/processors/inviwostatsplot.h>

#include <modules/opengl/texture/textureutils.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/openglutils.h>
#include <modules/opengl/rendering/meshdrawergl.h>
#include <modules/opengl/buffer/buffergl.h>

#include <inviwo/core/common/inviwoapplication.h>

#include <inviwo/core/util/performance/statisticscollector.h>
#include <inviwo/core/util/zip.h>
#include <modules/base/algorithm/randomutils.h>

#include <inviwo/core/util/colorbrewer.h>
#include <modules/plotting/utils/axisutils.h>

#include <algorithm>
#include <chrono>

#include <fmt/format.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo InviwoStatsPlot::processorInfo_{
    "org.inviwo.InviwoStatsPlot",  // Class identifier
    "Inviwo Stats Plot",           // Display name
    "Testing",                     // Category
    CodeState::Stable,             // Code state
    Tags::GL,                      // Tags
};
const ProcessorInfo InviwoStatsPlot::getProcessorInfo() const { return processorInfo_; }

InviwoStatsPlot::Lines::Lines(size_t size) : size{size} {
    glGenBuffers(static_cast<GLsizei>(bufferIds.size()), bufferIds.data());
    glGenVertexArrays(1, &vertexArrayId);

    glBindVertexArray(vertexArrayId);

    glEnableVertexAttribArray(Name::time);
    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::time]);
    glVertexAttribPointer(Name::time, 1, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(Name::duration);
    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::duration]);
    glVertexAttribPointer(Name::duration, 1, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(Name::color);
    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::color]);
    glVertexAttribPointer(Name::color, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(Name::pickid);
    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::pickid]);
    glVertexAttribIPointer(Name::pickid, 1, GL_UNSIGNED_INT, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    resize(size);
}

void InviwoStatsPlot::Lines::resize(size_t newSize) {
    size = newSize;
    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::time]);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::duration]);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::color]);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(vec4), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::pickid]);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InviwoStatsPlot::Lines::map() {
    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::time]);
    times = static_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::duration]);
    durations = static_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::color]);
    colors = static_cast<vec4*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::pickid]);
    pickids = static_cast<unsigned int*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void InviwoStatsPlot::Lines::unmap() {
    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::time]);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    times = nullptr;

    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::duration]);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    durations = nullptr;

    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::color]);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    colors = nullptr;

    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[Name::pickid]);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    pickids = nullptr;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InviwoStatsPlot::Lines::bind() { glBindVertexArray(vertexArrayId); }
void InviwoStatsPlot::Lines::unbind() { glBindVertexArray(0); }

InviwoStatsPlot::Lines::~Lines() {
    glDeleteVertexArrays(1, &vertexArrayId);
    glDeleteBuffers(static_cast<GLsizei>(bufferIds.size()), bufferIds.data());
}

InviwoStatsPlot::InviwoStatsPlot(InviwoApplication* app)
    : Processor()
    , app_{app}
    , backgroundPort_{"background"}
    , outport_{"outport"}
    , enable_("enable", "Enable", true)
    , timeWindow_("timeWindow", "Time Window (s)", 20.0, 0.0, 10000.0)
    , dataRange_("dataRange", "Data Range (ms)", 0.0, 100.0, 0.0, 2000.0)
    , position_{"position", "Position", size2_t(70), size2_t(0), size2_t(6000)}
    , size_{"size", "Size", size2_t(500), size2_t(0), size2_t(6000)}
    , timeAxisSettings_{}
    , durationAxisSettings_{}
    , timeAxis_{timeAxisSettings_}
    , durationAxis_{durationAxisSettings_}
    , shader_("stat_lines.vert", "stat_lines.geom", "stat_lines.frag", false)
    , lines_{32 * 1024}
    , pickingMapper_{this, 1, [&](PickingEvent* p) { linePicked(p); }} {

    addPort(backgroundPort_).setOptional(true);
    addPort(outport_);
    addProperties(enable_, timeWindow_, dataRange_, position_, size_);

    auto vs = shader_.getVertexShaderObject();
    vs->clearInDeclarations();
    vs->addInDeclaration("in_Time", 0, "float");
    vs->addInDeclaration("in_Duration", 1, "float");
    vs->addInDeclaration("in_Color", 2, "vec4");
    vs->addInDeclaration("in_Picking", 3, "uint");

    shader_.onReload([&]() { this->invalidate(InvalidationLevel::InvalidOutput); });
    shader_.build();

    timeAxisSettings_.labelSettings.offset = vec2{20.0f, 0.0f};
    timeAxisSettings_.labelSettings.font.anchorPos = vec2{0.0f, 1.0f};
    timeAxisSettings_.captionSettings.enabled = false;

    durationAxisSettings_.orientation = plot::AxisData::Orientation::Vertical;
    durationAxisSettings_.flipped = true;
    durationAxisSettings_.labelSettings.offset = vec2{20.0f, 0.0f};
    durationAxisSettings_.labelSettings.font.anchorPos = vec2{1.0f, 0.0f};
    durationAxisSettings_.captionSettings.enabled = false;
}

void InviwoStatsPlot::process() {
    if (!enable_) {
        outport_.setData(backgroundPort_.getData());
        return;
    }


    using Clock = std::chrono::high_resolution_clock;

    const auto now = Clock::now();
    const auto window = Clock::duration{static_cast<Clock::rep>(timeWindow_ * 1.0e9)};

    auto& indices = indices_.getEditableRAMRepresentation()->getDataContainer();
    indices.clear();

    auto& sc = app_->getStatisticsCollector();
    auto colormap = colorbrewer::getColormap(colorbrewer::Colormap::Spectral_5);

    const auto seriesKeys = sc.getSeries();
    pickingMapper_.resize(seriesKeys.size());

    std::vector<std::tuple<std::string, size_t>> iters;
    size_t nelem = 0;
    for (auto& seriesKey : seriesKeys) {
        const auto& timepoints = sc.getTimes(seriesKey);
        const auto it = std::lower_bound(timepoints.begin(), timepoints.end(), now - window);
        nelem += std::distance(it, timepoints.end());
        iters.emplace_back(seriesKey, std::distance(timepoints.begin(), it));
    }

    if (nelem > lines_.size) {
        lines_.resize(util::nextPow2(nelem));
    }

    size_t offset = 0;
    size_t seriesIndex = 0;
    uint32_t vertexCount = 0;
    lines_.map();
    for (auto& item : iters) {
        auto& key = std::get<0>(item);
        auto& skip = std::get<1>(item);

        auto& timepoints = sc.getTimes(key);
        const auto size = std::distance(timepoints.begin() + skip, timepoints.end());
        if (size > 0) {
            std::transform(
                timepoints.begin() + skip, timepoints.end(), lines_.times + offset,
                [&](Clock::time_point t) -> float { return (now - t).count() * 1.0e-9f; });

            auto& durations = sc.getDurations(key);
            std::transform(durations.begin() + skip, durations.end(), lines_.durations + offset,
                           [&](Clock::duration d) -> float { return d.count() * 1.0e-6f; });

            const auto color = colormap[seriesIndex % colormap.size()];
            std::fill(lines_.colors + offset, lines_.colors + offset + size, color);

            const auto pickId = static_cast<uint32_t>(pickingMapper_.getPickingId(seriesIndex));
            std::fill(lines_.pickids + offset, lines_.pickids + offset + size, pickId);

            std::generate_n(std::back_inserter(indices), size,
                            [&]() -> uint32_t { return vertexCount++; });
            indices.push_back(static_cast<uint32_t>(-1));
        }
        ++seriesIndex;
        offset += size;
    }
    lines_.unmap();

    const auto dims = outport_.getDimensions();
    utilgl::activateTargetAndClearOrCopySource(outport_, backgroundPort_);

    {
        glPrimitiveRestartIndex(static_cast<uint32_t>(-1));
        glEnable(GL_PRIMITIVE_RESTART);

        utilgl::ViewportState vp{
            static_cast<GLint>(position_.get().x), static_cast<GLint>(position_.get().y),
            static_cast<GLsizei>(size_.get().x), static_cast<GLsizei>(size_.get().y)};

        shader_.activate();

        lines_.bind();
        indices_.getRepresentation<BufferGL>()->bind();

        shader_.setUniform("dims", ivec2(size_.get()));
        shader_.setUniform("dataMin", vec2{0.0, dataRange_.getStart()});
        shader_.setUniform("dataMax", vec2{timeWindow_, dataRange_.getEnd()});
        shader_.setUniform("domainMin", ivec2{1, 1});
        shader_.setUniform("domainMax", ivec2{size_.get()} - ivec2{1, 1});

        glDrawElements(GL_LINE_STRIP, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT,
                       nullptr);
        shader_.deactivate();

        lines_.unbind();

        glDisable(GL_PRIMITIVE_RESTART);
    }

    const auto xEnd = size2_t{position_.get().x + size_.get().x, position_.get().y};
    const auto yEnd = size2_t{position_.get().x, position_.get().y + size_.get().y};
    const auto margin = size_t{10};
    {
        if (timeWindow_.isModified()) {
            timeAxisSettings_.range.x = 0.0;
            timeAxisSettings_.range.y = timeWindow_.get();

            const auto tickmarks =
                plot::getMajorTickPositions(timeAxisSettings_.majorTicks, timeAxisSettings_.range);
            timeAxisSettings_.labels.clear();
            std::transform(tickmarks.begin(), tickmarks.end(),
                           std::back_inserter(timeAxisSettings_.labels),
                           [&](auto tick) { return fmt::format("{}", tick); });
        }

        timeAxis_.render(dims, position_.get() - size2_t(0, margin), xEnd - size2_t(0, margin),
                         true);
    }
    {
        if (dataRange_.isModified()) {
            durationAxisSettings_.range.x = dataRange_.getStart();
            durationAxisSettings_.range.y = dataRange_.getEnd();

            const auto tickmarks = plot::getMajorTickPositions(durationAxisSettings_.majorTicks,
                                                               durationAxisSettings_.range);
            durationAxisSettings_.labels.clear();
            std::transform(tickmarks.begin(), tickmarks.end(),
                           std::back_inserter(durationAxisSettings_.labels),
                           [&](auto tick) { return fmt::format("{}", tick); });
        }
        durationAxis_.render(dims, position_.get() - size2_t(margin, 0), yEnd - size2_t(margin, 0),
                             true);
    }
    LGL_ERROR;
}

void InviwoStatsPlot::linePicked(PickingEvent* p) {
    // Show tooltip about current line
    if (p->getHoverState() == PickingHoverState::Move ||
        p->getHoverState() == PickingHoverState::Enter) {

        auto& sc = app_->getStatisticsCollector();
        const auto series = sc.getSeries();
        const auto pickId = p->getPickedId();
        if (pickId < series.size()) {
            const auto time = sc.getDurations(series[pickId]).back();
            p->setToolTip(fmt::format("{} {} ms", series[pickId], time.count() * 1.0e-6));
        }
    } else {
        p->setToolTip("");
    }
}

}  // namespace inviwo
