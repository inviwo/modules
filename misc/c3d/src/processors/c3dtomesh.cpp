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

#include <inviwo/c3d/processors/c3dtomesh.h>

#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/geometry/meshram.h>
#include <inviwo/core/interaction/events/pickingevent.h>

#include <ezc3d/ezc3d.h>
#include <ezc3d/Header.h>
#include <ezc3d/Data.h>

#include <cmath>
#include <numeric>

#include <fmt/format.h>

namespace inviwo {

const ProcessorInfo C3DToMesh::processorInfo_{
    "org.inviwo.C3DToMesh",   // Class identifier
    "C3D To Mesh",            // Display name
    "C3D",                    // Category
    CodeState::Experimental,  // Code state
    Tags::CPU | Tag{"C3D"} | Tag{"Mesh"},
    R"(Creates a point cloud mesh from C3D marker positions over a range of frames.

    Each 3D marker is represented as a point vertex. Multiple frames can be
    extracted simultaneously. Invalid or empty markers can optionally be skipped.
    Picking support provides tooltips with point name, frame, time, and position.
    )"_unindentHelp,
};
const ProcessorInfo& C3DToMesh::getProcessorInfo() const { return processorInfo_; }

C3DToMesh::C3DToMesh()
    : Processor{}
    , inport_{"inport", ""_help}
    , bnl_{"bnl"}
    , outport_{"outport", ""_help}
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
    , enableTooltips_{"enableTooltips", "Enable Tooltips",
                      "Show point name, frame, time, and position on hover"_help, true}
    , picking_{this, 1, [this](PickingEvent* e) { handlePicking(e); }} {

    addPorts(inport_, bnl_, outport_);
    addProperties(frame_, markerRadius_, skipEmpty_, enableTooltips_);

    inport_.onChange([this]() {
        if (inport_.hasData()) {
            const auto& header = inport_.getData()->data().header();
            const size_t nbFrames = header.nbFrames();
            const size_t maxFrame = nbFrames > 0 ? nbFrames - 1 : 0;
            frame_.setRangeMax(maxFrame);
        }
    });
}

void C3DToMesh::process() {
    data_ = inport_.getData();
    const auto& c3dData = *data_;
    const auto& c3d = c3dData.data();
    const auto& header = c3d.header();

    const size_t nbFrames = header.nbFrames();
    const size_t nbPoints = header.nb3dPoints();

    if (nbFrames == 0 || nbPoints == 0) {
        outport_.setData(std::make_shared<Mesh>());
        return;
    }

    const size_t startFrame = std::min(frame_.getStart(), nbFrames - 1);
    const size_t endFrame = std::min(frame_.getEnd(), nbFrames - 1);

    std::vector<vec3> positions;
    std::vector<vec4> colors;
    std::vector<float> radii;
    std::vector<uint32_t> index;
    std::vector<uint32_t> pickIds;

    const size_t totalFrames = endFrame - startFrame + 1;
    positions.reserve(nbPoints * totalFrames);
    colors.reserve(nbPoints * totalFrames);
    radii.reserve(nbPoints * totalFrames);
    index.reserve(nbPoints * totalFrames);
    pickIds.reserve(nbPoints * totalFrames);
    picking_.resize(std::max<size_t>(nbPoints * totalFrames, 1));

    for (size_t frameIdx = startFrame; frameIdx <= endFrame; ++frameIdx) {
        const auto& framePoints = c3d.data().frame(frameIdx).points();

        for (size_t pointIdx = 0; pointIdx < nbPoints; ++pointIdx) {
            const auto& point = framePoints.point(pointIdx);

            if (skipEmpty_ && point.residual() < 0.0) {
                continue;
            }

            const vec3 pos{static_cast<float>(point.x()), static_cast<float>(point.y()),
                           static_cast<float>(point.z())};

            positions.emplace_back(pos);

            // Assign a distinct color per marker using cosine-based hue distribution
            const float hue = static_cast<float>(pointIdx) / static_cast<float>(nbPoints);
            constexpr float tau = 6.28318f;
            colors.emplace_back(0.5f + 0.5f * std::cos(tau * (hue + 0.0f)),
                                0.5f + 0.5f * std::cos(tau * (hue + 0.333f)),
                                0.5f + 0.5f * std::cos(tau * (hue + 0.667f)), 1.0f);

            radii.emplace_back(markerRadius_.get());

            // Unique ID for each point
            pickIds.emplace_back(picking_.getPickingId(frameIdx * nbPoints + pointIdx));

            index.emplace_back(static_cast<uint32_t>(frameIdx));
        }
    }

    auto mesh = std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    mesh->addBuffer(BufferType::PositionAttrib, util::makeBuffer(std::move(positions)));
    mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(std::move(colors)));
    mesh->addBuffer(BufferType::RadiiAttrib, util::makeBuffer(std::move(radii)));
    mesh->addBuffer(BufferType::IndexAttrib, util::makeBuffer(std::move(index)));
    mesh->addBuffer(BufferType::PickingAttrib, util::makeBuffer(std::move(pickIds)));

    outport_.setData(mesh);
}

void C3DToMesh::handlePicking(PickingEvent* e) {
    if (!enableTooltips_) return;

    if (e->getPressState() == PickingPressState::None) {
        if (data_ && (e->getHoverState() == PickingHoverState::Move ||
                      e->getHoverState() == PickingHoverState::Enter)) {

            const auto id = e->getPickedId();

            const auto& c3d = data_->data();
            const auto& header = c3d.header();

            const size_t nbFrames = header.nbFrames();
            const size_t nbPoints = header.nb3dPoints();
            const float frameRate = header.frameRate();
            const auto pointNames = c3d.pointNames();

            const auto frameIdx = id / nbPoints;
            const auto pointIdx = id % nbPoints;

            if (frameIdx >= nbFrames || pointIdx >= pointNames.size()) return;

            const auto& name = pointNames[pointIdx];
            const float time = (frameRate > 0.0f) ? static_cast<float>(frameIdx) / frameRate : 0.0f;
            const auto& framePoints = c3d.data().frame(frameIdx).points();
            const auto& point = framePoints.point(pointIdx);

            e->setToolTip(fmt::format(
                "Point: {} ({})\nFrame: {}\nTime: {:.4f} s\nPosition: "
                "({:.2f}, {:.2f}, {:.2f})\nResidual: {:.2f}",
                name, pointIdx, frameIdx, time, point.x(), point.y(), point.z(), point.residual()));

            bnl_.highlight({frameIdx}, BrushingTarget::Row);
            bnl_.highlight({pointIdx}, BrushingTarget::Column);

        } else if (e->getHoverState() == PickingHoverState::Exit) {
            e->setToolTip("");

            bnl_.highlight({}, BrushingTarget::Row);
            bnl_.highlight({}, BrushingTarget::Column);
        }
    }
}

}  // namespace inviwo
