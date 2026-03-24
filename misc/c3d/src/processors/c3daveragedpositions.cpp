/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2026 Inviwo Foundation
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

#include <inviwo/c3d/processors/c3daveragedpositions.h>
#include <inviwo/core/interaction/events/pickingevent.h>

#include <ezc3d/ezc3d.h>
#include <ezc3d/Header.h>
#include <ezc3d/Data.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo C3DAveragedPositions::processorInfo_{
    "org.inviwo.C3DAveragedPositions",  // Class identifier
    "C3DAveraged Positions",            // Display name
    "Undefined",                        // Category
    CodeState::Experimental,            // Code state
    Tags::None,                         // Tags
    R"(<Explanation of how to use the processor.>)"_unindentHelp,
};

const ProcessorInfo& C3DAveragedPositions::getProcessorInfo() const { return processorInfo_; }

C3DAveragedPositions::C3DAveragedPositions()
    : Processor{}
    , inport_{"inport"}
    , meshOutport_{"outport"}
    , positionsOutport_{"positionsOutport"}
    , namesOutport_{"namesOutport"}
    , frame_{"frame", "Frame Range", "Range of frame indices to include in the mesh"_help, 0, 0, 0,
             0}
    , markerRadius_{"markerRadius",
                    "Marker Radius",
                    "Radius used for sphere glyphs when rendering"_help,
                    5.0f,
                    {0.1f, ConstraintBehavior::Immutable},
                    {100.0f, ConstraintBehavior::Mutable}}
    , skipEmpty_{"skipEmpty", "Skip Empty Points",
                 "Skip markers with negative residuals (indicating invalid data)"_help, true}
    , picking_{this, 1, [this](PickingEvent* e) { handlePicking(e); }}
    , lines_{"lines", "Lines", "", InvalidationLevel::InvalidOutput, PropertySemantics::Multiline}
    , triangles_("triangles", "Triangles", "", InvalidationLevel::InvalidOutput,
                 PropertySemantics::Multiline) {

    addPorts(inport_, meshOutport_, positionsOutport_, namesOutport_);
    addProperties(frame_, markerRadius_, skipEmpty_, lines_, triangles_);

    inport_.onChange([this]() {
        if (inport_.hasData()) {
            const auto& header = inport_.getData()->header();
            const size_t nbFrames = header.nbFrames();
            const size_t maxFrame = nbFrames > 0 ? nbFrames - 1 : 0;
            frame_.setRangeMax(maxFrame);
        }
    });
}

void C3DAveragedPositions::process() {
    const auto& c3d = *inport_.getData();
    const auto& header = c3d.header();
    const size_t nbFrames = header.nbFrames();
    const size_t nbPoints = header.nb3dPoints();

    if (nbFrames == 0 || nbPoints == 0) {
        meshOutport_.clear();
        positionsOutport_.clear();
        namesOutport_.clear();
        return;
    }

    const size_t startFrame = std::min(frame_.getStart(), nbFrames - 1);
    const size_t endFrame = std::min(frame_.getEnd(), nbFrames - 1);

    std::vector<vec3> positions;
    std::vector<size_t> counts;
    std::vector<vec4> colors;
    std::vector<float> radii;
    std::vector<uint32_t> index;
    std::vector<uint32_t> pickIds;

    positions.reserve(nbPoints);
    counts.reserve(nbPoints);
    colors.reserve(nbPoints);
    radii.reserve(nbPoints);
    index.reserve(nbPoints);
    pickIds.reserve(nbPoints);
    picking_.resize(std::max<size_t>(nbPoints, 1));

    for (size_t pointIdx = 0; pointIdx < nbPoints; ++pointIdx) {
        positions.emplace_back(vec3(0.0f));
        counts.emplace_back(0);

        // Assign a distinct color per marker using cosine-based hue distribution
        const float hue = static_cast<float>(pointIdx) / static_cast<float>(nbPoints);
        constexpr float tau = 6.28318f;
        colors.emplace_back(0.5f + 0.5f * std::cos(tau * (hue + 0.0f)),
                            0.5f + 0.5f * std::cos(tau * (hue + 0.333f)),
                            0.5f + 0.5f * std::cos(tau * (hue + 0.667f)), 1.0f);

        radii.emplace_back(markerRadius_.get());

        index.emplace_back(static_cast<uint32_t>(pointIdx));
        pickIds.emplace_back(picking_.getPickingId(pointIdx));
    }

    for (size_t frameIdx = startFrame; frameIdx <= endFrame; ++frameIdx) {
        const auto& framePoints = c3d.data().frame(frameIdx).points();

        for (size_t pointIdx = 0; pointIdx < nbPoints; ++pointIdx) {
            const auto& point = framePoints.point(pointIdx);
            if (skipEmpty_ && point.residual() < 0.0) continue;

            const vec3 pos{static_cast<float>(point.x()), static_cast<float>(point.y()),
                           static_cast<float>(point.z())};
            positions[pointIdx] += pos;
            counts[pointIdx] += 1;
        }
    }

    for (size_t pointIdx = 0; pointIdx < nbPoints; ++pointIdx) {
        positions[pointIdx] /= counts[pointIdx] > 0 ? static_cast<float>(counts[pointIdx]) : 1.0f;
    }

    positions_ = positions;
    names_ = c3d.pointNames();

    auto mesh = std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    mesh->addBuffer(BufferType::PositionAttrib, util::makeBuffer(std::move(positions)));
    mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(std::move(colors)));
    mesh->addBuffer(BufferType::RadiiAttrib, util::makeBuffer(std::move(radii)));
    mesh->addBuffer(BufferType::IndexAttrib, util::makeBuffer(std::move(index)));
    mesh->addBuffer(BufferType::PickingAttrib, util::makeBuffer(std::move(pickIds)));

    mesh->addIndices({.dt = DrawType::Points, .ct = ConnectivityType::None},
                     util::makeIndexBuffer(
                         std::views::iota(std::uint32_t{0}, static_cast<std::uint32_t>(nbPoints)) |
                         std::ranges::to<std::vector>()));

    if (!lines_.get().empty()) {
        mesh->addIndices(
            {.dt = DrawType::Lines, .ct = ConnectivityType::None},
            util::makeIndexBuffer(
                lines_.get() | std::views::split('\n') |
                std::views::transform([](auto&& line) { return std::string_view(line); }) |
                std::views::transform([&](std::string_view line) {
                    auto [l1, l2] = util::splitByFirst(line, ' ');
                    const auto i1 = c3d.pointIdx(std::string{l1});
                    const auto i2 = c3d.pointIdx(std::string{l2});
                    return std::array{i1, i2};
                }) |
                std::views::join |
                std::views::transform([](size_t i) { return static_cast<uint32_t>(i); }) |
                std::ranges::to<std::vector>()));
    }

    if (!triangles_.get().empty()) {
        mesh->addIndices(
            {.dt = DrawType::Triangles, .ct = ConnectivityType::None},
            util::makeIndexBuffer(
                triangles_.get() | std::views::split('\n') |
                std::views::transform([](auto&& line) { return std::string_view(line); }) |
                std::views::transform([&](std::string_view line) {
                    auto [l1, rest] = util::splitByFirst(line, ' ');
                    auto [l2, l3] = util::splitByFirst(rest, ' ');
                    const auto i1 = c3d.pointIdx(std::string{l1});
                    const auto i2 = c3d.pointIdx(std::string{l2});
                    const auto i3 = c3d.pointIdx(std::string{l3});
                    return std::array{i1, i2, i3};
                }) |
                std::views::join |
                std::views::transform([](size_t i) { return static_cast<uint32_t>(i); }) |
                std::ranges::to<std::vector>()));
    }

    meshOutport_.setData(mesh);
    positionsOutport_.setData(std::make_shared<std::vector<vec3>>(positions_));
    namesOutport_.setData(std::make_shared<std::vector<std::string>>(names_));
}

void C3DAveragedPositions::handlePicking(PickingEvent* e) {
    if (e->getPressState() == PickingPressState::None) {
        if ((e->getHoverState() == PickingHoverState::Move ||
             e->getHoverState() == PickingHoverState::Enter)) {

            const auto pointIdx = e->getPickedId();
            if (pointIdx < positions_.size()) {
                e->setToolTip(fmt::format("Point: {} ({})\nPosition: {}", names_[pointIdx],
                                          pointIdx, positions_[pointIdx]));
            }
        } else if (e->getHoverState() == PickingHoverState::Exit) {
            e->setToolTip("");
        }
    }
}
}  // namespace inviwo
