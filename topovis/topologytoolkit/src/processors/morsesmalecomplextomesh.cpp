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

#include <inviwo/topologytoolkit/processors/morsesmalecomplextomesh.h>

#include <modules/opengl/inviwoopengl.h>

#include <algorithm>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MorseSmaleComplexToMesh::processorInfo_{
    "org.inviwo.MorseSmaleComplexToMesh",  // Class identifier
    "Morse-Smale Complex To Mesh",         // Display name
    "Topology",                            // Category
    CodeState::Experimental,               // Code state
    "CPU, Topology, TTK, Contour Tree",    // Tags
};
const ProcessorInfo MorseSmaleComplexToMesh::getProcessorInfo() const { return processorInfo_; }

MorseSmaleComplexToMesh::MorseSmaleComplexToMesh()
    : Processor()
    , propColors_("colors", "Colors")
    , sphereRadius_("sphereRadius", "Radius", 0.05f, 0.0f, 10.0f) {
    addPort(mscInport_);
    addPort(outport_);

    addProperties(propColors_, sphereRadius_);
}

void MorseSmaleComplexToMesh::process() {
    auto pMSCData = mscInport_.getData();
    if (!pMSCData) return;

    // Prepare Memory
    const ttk::SimplexId numcp = pMSCData->criticalPoints.numberOfPoints;
    std::vector<vec3> positions(numcp);
    std::vector<vec4> colors(numcp, {1.0f, 1.0f, 1.0f, 1.0f});
    std::vector<float> radius(numcp, sphereRadius_.get());

    // Add critical points with their color
    for (ttk::SimplexId i = 0; i < numcp; i++) {
        positions[i].x = pMSCData->criticalPoints.points[3 * i];
        positions[i].y = pMSCData->criticalPoints.points[3 * i + 1];
        positions[i].z = pMSCData->criticalPoints.points[3 * i + 2];
    }

    if (pMSCData->triangulation->getTriangulation().getDimensionality() == 2) {
        std::transform(pMSCData->criticalPoints.cellDimensions.begin(),
                       pMSCData->criticalPoints.cellDimensions.end(), colors.begin(),
                       [this](char c) { return propColors_.getColor2D(c); });
    } else {
        std::transform(pMSCData->criticalPoints.cellDimensions.begin(),
                       pMSCData->criticalPoints.cellDimensions.end(), colors.begin(),
                       [this](char c) { return propColors_.getColor3D(c); });
    }

    // Add the separatrixCells
    const ttk::SimplexId idPrev = positions.size();

    ttk::SimplexId NumSepPoints = pMSCData->separatrixPoints.numberOfPoints;
    positions.reserve(positions.size() + NumSepPoints);
    colors.resize(colors.size() + NumSepPoints);
    radius.resize(radius.size() + NumSepPoints);

    std::fill(colors.begin() + idPrev, colors.end(), propColors_.arc_.get());
    std::fill(radius.begin() + idPrev, radius.end(), sphereRadius_.get());

    for (ttk::SimplexId i = 0; i < NumSepPoints; i++) {
        positions.push_back({pMSCData->separatrixPoints.points[3 * i],
                             pMSCData->separatrixPoints.points[3 * i + 1],
                             pMSCData->separatrixPoints.points[3 * i + 2]});
    }

    // - Separatrices
    std::vector<uint32_t> sepIndices;

    ttk::SimplexId currentCellId = -1;
    for (ttk::SimplexId i = 0; i < pMSCData->separatrixCells.numberOfCells; ++i) {
        // const ttk::SimplexId sourceId = pMSCData->separatrixCells.sourceIds[i];
        // const ttk::SimplexId destId = pMSCData->separatrixCells.destinationIds[i];

        // assuming that separatrixCells.cells[x + 0] holds the dimensionality, x + 1 and x + 2 hold
        // the from/to indices
        // assuming consecutive indices as well.

        auto from = pMSCData->separatrixCells.cells[3 * i + 1];
        auto to = pMSCData->separatrixCells.cells[3 * i + 2];

        if (currentCellId != pMSCData->separatrixCells.separatrixIds[i]) {
            if (i > 0) {
                sepIndices.push_back(0xffffffff);
            }
            currentCellId = pMSCData->separatrixCells.separatrixIds[i];
            sepIndices.push_back(from + idPrev);
        }
        sepIndices.push_back(to + idPrev);
    }

    auto mesh = std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    mesh->addBuffer(BufferType::PositionAttrib, util::makeBuffer(std::move(positions)));
    mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(std::move(colors)));
    mesh->addBuffer(BufferType::RadiiAttrib, util::makeBuffer(std::move(radius)));
    // - critical points
    std::vector<uint32_t> cpIndices(numcp);
    std::iota(cpIndices.begin(), cpIndices.end(), 0);
    mesh->addIndicies(Mesh::MeshInfo(DrawType::Points, ConnectivityType::None),
                      util::makeIndexBuffer(std::move(cpIndices)));
    // - separatrixCells
    mesh->addIndicies(Mesh::MeshInfo(DrawType::Lines, ConnectivityType::Strip),
                      util::makeIndexBuffer(std::move(sepIndices)));
    // vertex positions are already transformed
    mesh->setModelMatrix(mat4(1.0f));
    mesh->setWorldMatrix(pMSCData->triangulation->getWorldMatrix());
    mesh->copyMetaDataFrom(*pMSCData->triangulation);

    // Hack: enable primitive restart so we need only a single index buffer for multiple lines
    // The available mesh renderers do no support primitive restart, yet. This will work unless some
    // other processor disables it or uses a different primitive restart ID.
    glPrimitiveRestartIndex(0xffffffff);
    glEnable(GL_PRIMITIVE_RESTART);

    outport_.setData(mesh);
}

}  // namespace inviwo
