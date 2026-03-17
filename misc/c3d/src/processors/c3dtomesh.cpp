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

#include <ezc3d/ezc3d.h>

#include <cmath>

namespace inviwo {

const ProcessorInfo C3DToMesh::processorInfo_{
    "org.inviwo.C3DToMesh",   // Class identifier
    "C3D To Mesh",             // Display name
    "C3D",                     // Category
    CodeState::Experimental,   // Code state
    Tags::CPU | Tag{"C3D"} | Tag{"Mesh"},
    R"(Creates a point cloud mesh from C3D marker positions at a given frame.

    Each 3D marker is represented as a point vertex. Invalid or empty markers
    can optionally be skipped.
    )"_unindentHelp,
};
const ProcessorInfo& C3DToMesh::getProcessorInfo() const { return processorInfo_; }

C3DToMesh::C3DToMesh()
    : Processor{}
    , inport_{"inport", ""_help}
    , outport_{"outport", ""_help}
    , frame_{"frame", "Frame", "Frame index to visualize"_help, 0,
             {0, ConstraintBehavior::Immutable}, {0, ConstraintBehavior::Mutable}}
    , markerRadius_{"markerRadius", "Marker Radius",
                    "Radius used for sphere glyphs when rendering"_help, 5.0f,
                    {0.1f, ConstraintBehavior::Immutable}, {100.0f, ConstraintBehavior::Mutable}}
    , skipEmpty_{"skipEmpty", "Skip Empty Points",
                 "Skip markers with negative residuals (indicating invalid data)"_help, true} {

    addPorts(inport_, outport_);
    addProperties(frame_, markerRadius_, skipEmpty_);

    inport_.onChange([this]() {
        if (inport_.hasData()) {
            const auto& header = inport_.getData()->data().header();
            const size_t nbFrames = header.nbFrames();
            frame_.setMaxValue(nbFrames > 0 ? nbFrames - 1 : 0);
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

    const size_t frameIdx = std::min(frame_.get(), nbFrames - 1);
    const auto& framePoints = c3d.data().frame(frameIdx).points();

    std::vector<vec3> positions;
    std::vector<vec4> colors;
    std::vector<float> radii;

    positions.reserve(nbPoints);
    colors.reserve(nbPoints);
    radii.reserve(nbPoints);

    for (size_t p = 0; p < nbPoints; ++p) {
        const auto& point = framePoints.point(p);

        if (skipEmpty_ && point.residual() < 0.0) {
            continue;
        }

        positions.emplace_back(static_cast<float>(point.x()), static_cast<float>(point.y()),
                               static_cast<float>(point.z()));

        // Assign a distinct color per marker using cosine-based hue distribution
        const float hue = static_cast<float>(p) / static_cast<float>(nbPoints);
        constexpr float tau = 6.28318f;
        colors.emplace_back(0.5f + 0.5f * std::cos(tau * (hue + 0.0f)),
                            0.5f + 0.5f * std::cos(tau * (hue + 0.333f)),
                            0.5f + 0.5f * std::cos(tau * (hue + 0.667f)), 1.0f);

        radii.emplace_back(markerRadius_.get());
    }

    auto mesh = std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    mesh->addBuffer(BufferType::PositionAttrib, util::makeBuffer(std::move(positions)));
    mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(std::move(colors)));
    mesh->addBuffer(BufferType::RadiiAttrib, util::makeBuffer(std::move(radii)));

    outport_.setData(mesh);
}

}  // namespace inviwo
