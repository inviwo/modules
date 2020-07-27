/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/vortexfeatures/processors/vortextomesh.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <random>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VortexToMesh::processorInfo_{
    "org.inviwo.VortexToMesh",  // Class identifier
    "Vortex To Mesh",           // Display name
    "Undefined",                // Category
    CodeState::Experimental,    // Code state
    Tags::None,                 // Tags
};
const ProcessorInfo VortexToMesh::getProcessorInfo() const { return processorInfo_; }

VortexToMesh::VortexToMesh()
    : vorticesIn_("vorticesIn")
    , boundaryOut_("boundaries")
    , centerOut_("centers")
    , boundaryRep_("boundaryRep", "Boundary Mesh",
                   {{"lines", "Lines", BoundaryRepresentation::Lines},
                    {"meshes", "Surface", BoundaryRepresentation::Meshes}})
    , centerRep_("centerRep", "Center Mesh",
                 {{"points", "Points", CenterRepresentation::Points},
                  {"lines", "Lines", CenterRepresentation::Lines}})
    , showSlices3D_("showSLices3D", "Show 3D?",
                    {{"none", "None", ShowSlices3D::None},
                     {"time", "Time", ShowSlices3D::Time},
                     {"height", "Height", ShowSlices3D::Height}})
    , groupDisplay_("groupDisplay", "Group Display",
                    {
                        {"all", "All", GroupDisplay::All},
                        {"hightlightFirst", "Highlight First", GroupDisplay::HighlightFirst},
                        {"onlyFirst", "Only First", GroupDisplay::OnlyFirst},
                    })
    , timeSlice_("timeSlice", "Time Slice", 0, 0, 1, 1)
    , heightSlice_("heightSlice", "Height Slice", 0, 0, 1, 1)
    , overrideScaleZ_("overrideScaleZ", "Override Z Scale?", false)
    , scaleZ_("scaleZ", "Scalze Z", 0.01, 0, 1, 0.001) {

    addPort(vorticesIn_);
    addPort(boundaryOut_);
    addPort(centerOut_);
    addProperties(boundaryRep_, centerRep_, showSlices3D_, groupDisplay_, timeSlice_, heightSlice_,
                  overrideScaleZ_, scaleZ_);
    timeSlice_.visibilityDependsOn(showSlices3D_,
                                   [](auto& val) { return val != ShowSlices3D::Time; });
    heightSlice_.visibilityDependsOn(showSlices3D_,
                                     [](auto& val) { return val != ShowSlices3D::Height; });
    scaleZ_.visibilityDependsOn(overrideScaleZ_, [](bool val) { return val; });
    vorticesIn_.onChange([this]() {
        if (!vorticesIn_.hasData()) return;
        auto vortices = vorticesIn_.getData();
        timeSlice_.set(timeSlice_.get(), vortices->getTimeRange().x, vortices->getTimeRange().y, 1);
        heightSlice_.set(heightSlice_.get(), vortices->getHeightRange().x,
                         vortices->getHeightRange().y, 1);
    });
}

void VortexToMesh::process() {
    if (!vorticesIn_.hasData()) return;
    auto vortices = vorticesIn_.getData();
    if (vortices->size() == 0) return;

    auto boundMesh = std::make_shared<BasicMesh>();
    std::vector<BasicMesh::Vertex> boundPoints;

    // Random color generation.
    static std::default_random_engine eng;
    static std::uniform_real_distribution<float> rndDist(0, 1);
    std::vector<vec4> colors(vortices->numGroups());
    for (vec4& col : colors) {
        vec3 baseCol(rndDist(eng), rndDist(eng), rndDist(eng));
        baseCol = glm::normalize(baseCol);
        col = vec4(baseCol.x, baseCol.y, baseCol.z, 1);
    }
    mat4 modelMat = vortices->getModelMatrix();
    mat4 worldMat = vortices->getWorldMatrix();
    if (overrideScaleZ_) {
        modelMat[2][2] = scaleZ_.get();
        worldMat[2][2] = scaleZ_.get();
    }

    if (centerRep_.get() == CenterRepresentation::Points) {
        createPointCenterMesh(colors, modelMat, worldMat);
    } else {
        createLineCenterMesh(colors, modelMat, worldMat);
    }

    if (boundaryRep_.get() == BoundaryRepresentation::Lines) {
        createLineBoundaryMesh(colors, modelMat, worldMat);
    } else {
        createSurfaceBoundaryMesh(colors, modelMat, worldMat);
    }
}

bool VortexToMesh::keepVortex(const Vortex& vort) const {
    switch (showSlices3D_.get()) {
        case ShowSlices3D::Height:
            return vort.timeSlice == timeSlice_.get();
        case ShowSlices3D::Time:
            return vort.heightSlice == heightSlice_.get();
        default:
            return vort.timeSlice == timeSlice_.get() && vort.heightSlice == heightSlice_.get();
    }
}

size_t VortexToMesh::getPos3D(const Vortex& vort) const {
    switch (showSlices3D_.get()) {
        case ShowSlices3D::Height:
            return vort.heightSlice;
        case ShowSlices3D::Time:
            return vort.timeSlice;
        default:
            return 0;
    }
}

void VortexToMesh::createLineBoundaryMesh(const std::vector<vec4>& colors, const mat4& modelMat,
                                          const mat4& worldMat) {
    if (!vorticesIn_.hasData()) return;
    auto vortices = vorticesIn_.getData();

    // Boundary mesh.
    std::vector<ColoredMesh::Vertex> boundaryVerts;
    auto boundaryMesh = std::make_shared<ColoredMesh>(DrawType::Lines, ConnectivityType::None);
    auto indexBufferBoundary =
        boundaryMesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None);
    auto& boundaryIndices = indexBufferBoundary->getDataContainer();
    boundaryMesh->setModelMatrix(modelMat);
    boundaryMesh->setWorldMatrix(worldMat);

    size_t numVerts = 0;
    for (size_t group = 0; group < vortices->numGroups(); ++group) {
        bool fullColor = true;
        for (auto vort = vortices->beginGroup(group); vort != vortices->endGroup(group); ++vort) {
            if (!keepVortex(*vort)) continue;
            vec4 color = fullColor ? colors[group] : colors[group] * 0.5;
            color.a = 1;
            size_t valZ = getPos3D(*vort);
            for (const auto& point : vort->boundary) {
                boundaryVerts.push_back({vec3(point.x, point.y, valZ), color});
            }

            for (size_t line = 1; line < vort->boundary.size(); ++line) {
                boundaryIndices.push_back(numVerts + line - 1);
                boundaryIndices.push_back(numVerts + line);
            }
            if (groupDisplay_.get() == GroupDisplay::HighlightFirst) {
                fullColor = false;
            }
            numVerts += vort->size();
            if (groupDisplay_.get() == GroupDisplay::OnlyFirst) break;
        }
    }
    boundaryMesh->addVertices(boundaryVerts);
    boundaryOut_.setData(boundaryMesh);
}

void VortexToMesh::createSurfaceBoundaryMesh(const std::vector<vec4>& colors, const mat4& modelMat,
                                             const mat4& worldMat) {
    if (!vorticesIn_.hasData()) return;
    if (groupDisplay_.get() == GroupDisplay::OnlyFirst) {
        createLineBoundaryMesh(colors, modelMat, worldMat);
        return;
    }
    auto vortices = vorticesIn_.getData();

    // Boundary mesh.
    std::vector<ColoredMesh::Vertex> boundaryVerts;
    auto boundaryMesh = std::make_shared<ColoredMesh>(DrawType::Triangles, ConnectivityType::None);
    auto indexBufferBoundary =
        boundaryMesh->addIndexBuffer(DrawType::Triangles, ConnectivityType::None);
    auto& boundaryIndices = indexBufferBoundary->getDataContainer();
    boundaryMesh->setModelMatrix(modelMat);
    boundaryMesh->setWorldMatrix(worldMat);

    size_t firstVertPreviousLine = 0;
    size_t firstVertCurrentLine = 0;
    for (size_t group = 0; group < vortices->numGroups(); ++group) {
        vec4 color = colors[group];
        const Vortex* previousLine = nullptr;
        for (auto vort = vortices->beginGroup(group); vort != vortices->endGroup(group); ++vort) {
            if (!keepVortex(*vort)) continue;
            color.a = 1;
            size_t valZ = getPos3D(*vort);
            for (const auto& point : vort->boundary) {
                boundaryVerts.push_back({vec3(point.x, point.y, valZ), color});
            }

            if (color.a == 1 && groupDisplay_.get() == GroupDisplay::HighlightFirst) {
                color *= 0.5;
                color.a = 0.99;
            }

            if (!previousLine) continue;
            // Connect to last line by strip.
            size_t initCurrent =
                std::min_element(vort->boundary.begin(), vort->boundary.end(),
                                 [&](const dvec2& a, const dvec2& b) {
                                     return glm::distance2(a, previousLine->boundary[0]) <
                                            glm::distance2(b, previousLine->boundary[0]);
                                 }) -
                vort->boundary.begin();
            size_t currIdx = 0;
            size_t prevIdx = initCurrent;
            size_t currNext = 1;
            size_t prevNext = (prevIdx + 1) % previousLine->size();
            double distCurrNext =
                glm::distance2(previousLine->boundary[prevIdx], vort->boundary[currNext]);
            double distPrevNext =
                glm::distance2(previousLine->boundary[currIdx], vort->boundary[prevNext]);
            do {
                boundaryIndices.push_back(prevIdx);
                boundaryIndices.push_back(currIdx);
                if (distCurrNext <= distPrevNext) {
                    boundaryIndices.push_back(currNext);

                    currIdx = currNext;
                    if (currIdx == 0) {
                        distCurrNext = std::numeric_limits<double>::max();
                    } else {
                        currNext = (currIdx + 1) % vort->size();
                        distCurrNext = glm::distance2(previousLine->boundary[prevIdx],
                                                      vort->boundary[currNext]);
                    }
                } else {
                    boundaryIndices.push_back(prevNext);

                    prevIdx = prevNext;
                    if (prevIdx == initCurrent) {
                        distPrevNext = std::numeric_limits<double>::max();
                    } else {
                        prevNext = (prevIdx + 1) % previousLine->size();
                        distPrevNext = glm::distance2(previousLine->boundary[currIdx],
                                                      vort->boundary[prevNext]);
                    }
                }
            } while (currIdx != 0 && prevIdx != initCurrent);

            firstVertPreviousLine = firstVertCurrentLine;
            firstVertCurrentLine = boundaryIndices.size();
            previousLine = &*vort;
        }
    }
    boundaryMesh->addVertices(boundaryVerts);
    boundaryOut_.setData(boundaryMesh);
}

void VortexToMesh::createPointCenterMesh(const std::vector<vec4>& colors, const mat4& modelMat,
                                         const mat4& worldMat) {
    if (!vorticesIn_.hasData()) return;
    auto vortices = vorticesIn_.getData();

    // Mesh setup, center clustering.
    std::vector<SphereMesh::Vertex> centerPointVerts;
    auto centerMesh = std::make_shared<SphereMesh>(DrawType::Points, ConnectivityType::None);
    centerMesh->setModelMatrix(modelMat);
    centerMesh->setWorldMatrix(worldMat);

    for (size_t group = 0; group < vortices->numGroups(); ++group) {
        size_t numVorts = 0;
        for (auto vort = vortices->beginGroup(group); vort != vortices->endGroup(group); ++vort) {
            if (!keepVortex(*vort)) continue;
            numVorts++;
            size_t valZ = getPos3D(*vort);
            // for (const auto& point : vort->boundary) {
            centerPointVerts.push_back(
                {vec3(vort->center.x, vort->center.y, valZ), 0.002, colors[group]});
            if (groupDisplay_.get() == GroupDisplay::OnlyFirst) break;
        }
        if (groupDisplay_.get() == GroupDisplay::HighlightFirst) {
            std::get<1>(centerPointVerts[centerPointVerts.size() - numVorts]) *= 2;
        }
    }
    centerMesh->addVertices(centerPointVerts);
    centerOut_.setData(centerMesh);
}

void VortexToMesh::createLineCenterMesh(const std::vector<vec4>& colors, const mat4& modelMat,
                                        const mat4& worldMat) {
    if (!vorticesIn_.hasData()) return;
    auto vortices = vorticesIn_.getData();

    // Mesh setup, center clustering.
    std::vector<ColoredMesh::Vertex> centerPointVerts;
    auto centerMesh = std::make_shared<ColoredMesh>(DrawType::Lines, ConnectivityType::None);
    auto indexBuffer = centerMesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None);
    auto& centerIndices = indexBuffer->getDataContainer();

    centerMesh->setModelMatrix(modelMat);
    centerMesh->setWorldMatrix(worldMat);

    size_t numCenters = 0;
    bool isFirst = true;
    for (size_t group = 0; group < vortices->numGroups(); ++group) {
        for (auto vort = vortices->beginGroup(group); vort != vortices->endGroup(group); ++vort) {
            if (!keepVortex(*vort)) continue;
            if (!isFirst) {
                centerIndices.push_back(numCenters);
                centerIndices.push_back(++numCenters);
            }
            size_t valZ = getPos3D(*vort);
            // for (const auto& point : vort->boundary) {
            centerPointVerts.push_back({vec3(vort->center.x, vort->center.y, valZ), colors[group]});
            isFirst = false;
        }
        numCenters++;
    }
    centerMesh->addVertices(centerPointVerts);
    centerOut_.setData(centerMesh);
}
}  // namespace inviwo
