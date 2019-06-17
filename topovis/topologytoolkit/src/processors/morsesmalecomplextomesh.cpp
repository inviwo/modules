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
    , portInMSComplex("InMSComplex")
    , portOutMesh("OutMesh")
    , propColors_("colors", "Colors")
    , sphereRadius_("sphereRadius", "Radius", 0.05f, 0.0f, 10.0f)
{
    addPort(portInMSComplex);
    addPort(portOutMesh);

    addProperties(propColors_, sphereRadius_);
}

void MorseSmaleComplexToMesh::process() {
    // Get Input
    auto pMSCData = portInMSComplex.getData();
    if (!pMSCData) return;
    auto InTriangulation = pMSCData->triangulation;

    // Prepare Memory
    const ttk::SimplexId NumCP = pMSCData->criticalPoints.numberOfPoints;
    std::vector<vec3> positions(NumCP);
    std::vector<vec4> colors(NumCP, {1.0f, 1.0f, 1.0f, 1.0f});
    std::vector<float> radius(NumCP, sphereRadius_.get());
    // std::fill(radius.begin(), radius.begin() + tree->getNumberOfNodes(), sphereRadius_.get());

    // Add critical points with their color
    for (ttk::SimplexId i = 0; i < NumCP; i++) {
        positions[i].x = pMSCData->criticalPoints.points[3 * i];
        positions[i].y = pMSCData->criticalPoints.points[3 * i + 1];
        positions[i].z = pMSCData->criticalPoints.points[3 * i + 2];

        switch (pMSCData->criticalPoints.points_cellDimensions[i]) {
            case 0: {
                colors[i] = propColors_.localMinima_.get();
                // LogInfo("Min");
                break;
            }
            case 1: {
                colors[i] = propColors_.saddle_.get();
                // LogInfo("Saddle");
                break;
            }
            case 2: {
                if (pMSCData->triangulation->getTriangulation().getDimensionality() == 2) {
                    colors[i] = propColors_.localMaxima_.get();
                    // LogInfo("Max");
                } else {
                    colors[i] = propColors_.saddle_.get();
                    // LogInfo("Saddle");
                }

                break;
            }
            case 3: {
                ivwAssert(pMSCData->triangulation->getTriangulation().getDimensionality() == 3,
                          "A critical point from a cell type of 3 can only appear in 3D or higher "
                          "dimensional spaces.");
                colors[i] = propColors_.localMaxima_.get();
                break;
            }
        }
    }

    // Add the separatrices
    const ttk::SimplexId idPrev = positions.size();

    ttk::SimplexId NumSepPoints = pMSCData->separatrices.numberOfPoints;
    positions.reserve(positions.size() + NumSepPoints);
    colors.resize(colors.size() + NumSepPoints);
    radius.resize(radius.size() + NumSepPoints);

    std::fill(colors.begin() + idPrev, colors.end(), propColors_.arc_.get());
    std::fill(radius.begin() + idPrev, radius.end(), sphereRadius_.get());

    for (ttk::SimplexId i = 0; i < NumSepPoints; i++) {
        positions.push_back({pMSCData->separatrices.points[3 * i],
                             pMSCData->separatrices.points[3 * i + 1],
                             pMSCData->separatrices.points[3 * i + 2]});
        // LogInfo(pMSCData->separatrices.cells_separatrixIds[i]);
    }

    auto mesh = std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    mesh->addBuffer(BufferType::PositionAttrib, util::makeBuffer(std::move(positions)));
    mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(std::move(colors)));
    mesh->addBuffer(BufferType::RadiiAttrib, util::makeBuffer(std::move(radius)));
    // - CP
    std::vector<uint32_t> CPIndices(NumCP);
    std::iota(CPIndices.begin(), CPIndices.end(), 0);
    mesh->addIndicies(Mesh::MeshInfo(DrawType::Points, ConnectivityType::None),
                      util::makeIndexBuffer(std::move(CPIndices)));
    // - Sep
    std::vector<uint32_t> SepIndices; //(NumSepPoints);
    //std::iota(SepIndices.begin(), SepIndices.end(), idPrev);

    ttk::SimplexId currentCellId = -1;
    for (ttk::SimplexId i = 0; i < pMSCData->separatrices.numberOfCells; ++i) {
        const ttk::SimplexId sourceId = pMSCData->separatrices.cells_sourceIds[i];
        const ttk::SimplexId destId = pMSCData->separatrices.cells_destinationIds[i];

        // assuming that separatrices.cells[x + 0] holds the dimensionality, x + 1 and x + 2 hold
        // the from/to indices
        // assuming consecutive indices as well.

        auto from = pMSCData->separatrices.cells[3 * i + 1];
        auto to = pMSCData->separatrices.cells[3 * i + 2];

        if (currentCellId != pMSCData->separatrices.cells_separatrixIds[i]) {
            if (i > 0) {
                SepIndices.push_back(0xffffffff);
            }
            currentCellId = pMSCData->separatrices.cells_separatrixIds[i];
            SepIndices.push_back(from + idPrev);
        }
        SepIndices.push_back(to + idPrev);
    }

    mesh->addIndicies(Mesh::MeshInfo(DrawType::Lines, ConnectivityType::Strip),
                      util::makeIndexBuffer(std::move(SepIndices)));

    // enable primitive restart so we need only a single index buffer for multiple lines
    glPrimitiveRestartIndex(0xffffffff);
    glEnable(GL_PRIMITIVE_RESTART);

    portOutMesh.setData(mesh);
}

}  // namespace inviwo
