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

#include <cmath>
#include <numeric>

#include <fmt/format.h>

namespace inviwo {

const ProcessorInfo C3DToMesh::processorInfo_{
    "org.inviwo.C3DToMesh",   // Class identifier
    "C3D To Mesh",             // Display name
    "C3D",                     // Category
    CodeState::Experimental,   // Code state
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
    , outport_{"outport", ""_help}
    , frame_{"frame", "Frame Range", "Range of frame indices to include in the mesh"_help, 0, 0, 0,
             0}
    , markerRadius_{"markerRadius", "Marker Radius",
                    "Radius used for sphere glyphs when rendering"_help, 5.0f,
                    {0.1f, ConstraintBehavior::Immutable}, {100.0f, ConstraintBehavior::Mutable}}
    , skipEmpty_{"skipEmpty", "Skip Empty Points",
                 "Skip markers with negative residuals (indicating invalid data)"_help, true}
    , enableTooltips_{"enableTooltips", "Enable Tooltips",
                      "Show point name, frame, time, and position on hover"_help, true}
    , picking_{this, 1, [this](PickingEvent* e) { handlePicking(e); }} {

    addPorts(inport_, outport_);
    addProperties(frame_, markerRadius_, skipEmpty_, enableTooltips_);

    inport_.onChange([this]() {
        if (inport_.hasData()) {
            const auto& header = inport_.getData()->data().header();
            const size_t nbFrames = header.nbFrames();
            const size_t maxFrame = nbFrames > 0 ? nbFrames - 1 : 0;
            frame_.setRangeMax(maxFrame);
            frame_.setEnd(maxFrame);
        }
    });
}

void C3DToMesh::process() {
    const auto& c3dData = *inport_.getData();
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
    const float frameRate = header.frameRate();
    const auto pointNames = c3d.pointNames();

    std::vector<vec3> positions;
    std::vector<vec4> colors;
    std::vector<float> radii;
    std::vector<VertexInfo> info;

    const size_t totalFrames = endFrame - startFrame + 1;
    positions.reserve(nbPoints * totalFrames);
    colors.reserve(nbPoints * totalFrames);
    radii.reserve(nbPoints * totalFrames);
    info.reserve(nbPoints * totalFrames);

    for (size_t frameIdx = startFrame; frameIdx <= endFrame; ++frameIdx) {
        const auto& framePoints = c3d.data().frame(frameIdx).points();

        for (size_t p = 0; p < nbPoints; ++p) {
            const auto& point = framePoints.point(p);

            if (skipEmpty_ && point.residual() < 0.0) {
                continue;
            }

            const vec3 pos{static_cast<float>(point.x()), static_cast<float>(point.y()),
                           static_cast<float>(point.z())};

            positions.emplace_back(pos);

            // Assign a distinct color per marker using cosine-based hue distribution
            const float hue = static_cast<float>(p) / static_cast<float>(nbPoints);
            constexpr float tau = 6.28318f;
            colors.emplace_back(0.5f + 0.5f * std::cos(tau * (hue + 0.0f)),
                                0.5f + 0.5f * std::cos(tau * (hue + 0.333f)),
                                0.5f + 0.5f * std::cos(tau * (hue + 0.667f)), 1.0f);

            radii.emplace_back(markerRadius_.get());

            const std::string& name =
                (p < pointNames.size()) ? pointNames[p] : fmt::format("Point_{}", p);
            const float time =
                (frameRate > 0.0f) ? static_cast<float>(frameIdx) / frameRate : 0.0f;
            info.push_back({name, frameIdx, time, pos});
        }
    }

    const size_t vertexCount = positions.size();
    picking_.resize(std::max<size_t>(vertexCount, 1));
    vertexInfo_ = std::move(info);

    auto mesh = std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    mesh->addBuffer(BufferType::PositionAttrib, util::makeBuffer(std::move(positions)));
    mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(std::move(colors)));
    mesh->addBuffer(BufferType::RadiiAttrib, util::makeBuffer(std::move(radii)));

    if (enableTooltips_ && vertexCount > 0) {
        std::vector<uint32_t> pickIds(vertexCount);
        std::iota(pickIds.begin(), pickIds.end(),
                  static_cast<uint32_t>(picking_.getPickingId(0)));
        mesh->addBuffer(BufferType::PickingAttrib, util::makeBuffer(std::move(pickIds)));
    }

    outport_.setData(mesh);
}

void C3DToMesh::handlePicking(PickingEvent* e) {
    if (!enableTooltips_) return;

    if (e->getPressState() == PickingPressState::None) {
        if (e->getHoverState() == PickingHoverState::Move ||
            e->getHoverState() == PickingHoverState::Enter) {

            const auto id = e->getPickedId();
            if (id < vertexInfo_.size()) {
                const auto& vi = vertexInfo_[id];
                e->setToolTip(fmt::format("Point: {}\nFrame: {}\nTime: {:.4f} s\nPosition: "
                                          "({:.2f}, {:.2f}, {:.2f})",
                                          vi.name, vi.frame, vi.time, vi.position.x,
                                          vi.position.y, vi.position.z));
            }
        } else if (e->getHoverState() == PickingHoverState::Exit) {
            e->setToolTip("");
        }
    }
}

}  // namespace inviwo
