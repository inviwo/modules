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
#include <inviwo/core/interaction/events/pickingevent.h>
#include <inviwo/core/interaction/events/mouseevent.h>
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
    , selectionIn_("selection")
    , dataFrameIn_("dataFrame")
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
                    {{"all", "All", GroupDisplay::All},
                     {"hightlightFirst", "Highlight First", GroupDisplay::HighlightFirst},
                     {"onlyFirst", "Only First", GroupDisplay::OnlyFirst},
                     {"score", "Show Score", GroupDisplay::Score}})
    , colorSource_(
          "colorSource", "Color Source",
          {{"group", "Group", ColorSource::Group}, {"rotation", "Rotation", ColorSource::Rotation}})
    , selectionHighlight_("selectionHighlight", "Selection Highlight",
                          {{"ignore", "Ignore", SelectionHighlight::Ignore},
                           {"FilterSelect", "Filter & Select", SelectionHighlight::FilterSelect}})
    , colorSeed_("colorSeed", "Color Seed", 1, 0, 10000, 1)
    , ensembleMember_("ensembleMember", "Member", 1, 1, 50, 1)
    , timeSlice_("timeSlice", "Time Slice", 1, 1, 1, 1)
    , heightSlice_("heightSlice", "Height Slice", 1, 1, 1, 1)
    , singleGroupProperty_("singleGroupProperty", "Single Group?")
    , overrideScaleZ_("overrideScaleZ", "Override Z Scale?", false)
    , scaleZ_("scaleZ", "Scale Z", 0.1, -1, 1, 0.001)
    , skipLastGroup_("discardLastGroup", "Skip Last Group", false)
    , picking_(this, 1, [&](PickingEvent* p) { objectPicked(p); }) {

    addPort(vorticesIn_);
    addPort(selectionIn_);
    addPort(dataFrameIn_);
    selectionIn_.setOptional(true);
    dataFrameIn_.setOptional(true);
    addPort(boundaryOut_);
    addPort(centerOut_);
    addProperties(boundaryRep_, centerRep_, showSlices3D_, groupDisplay_, colorSource_,
                  selectionHighlight_, colorSeed_, ensembleMember_, timeSlice_, heightSlice_,
                  singleGroupProperty_, overrideScaleZ_, scaleZ_, skipLastGroup_);
    timeSlice_.visibilityDependsOn(showSlices3D_,
                                   [](auto& val) { return val != ShowSlices3D::Time; });
    heightSlice_.visibilityDependsOn(showSlices3D_,
                                     [](auto& val) { return val != ShowSlices3D::Height; });
    scaleZ_.visibilityDependsOn(overrideScaleZ_, [](bool val) { return val; });
    vorticesIn_.onChange([this]() {
        if (!vorticesIn_.hasData()) return;
        auto vortices = vorticesIn_.getData();
        timeSlice_.set(timeSlice_.get(), vortices->getTimeRange().x + 1,
                       vortices->getTimeRange().y + 1, 1);
        heightSlice_.set(heightSlice_.get(), vortices->getHeightRange().x + 1,
                         vortices->getHeightRange().y + 1, 1);
        singleGroupProperty_.selectedGroup_.setMaxValue(vortices->numGroups() - 1);
        picking_.resize(vortices->numGroups() + 1);
        LogWarn("picking_.getPickingId(0)   =   " << picking_.getPickingId(0));
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
    eng.seed(colorSeed_.get());
    static std::uniform_real_distribution<float> rndDist(0, 1);
    std::vector<vec4> colors(vortices->numGroups());
    for (size_t idx = 0; idx < vortices->numGroups(); ++idx) {
        vec3 baseCol(rndDist(eng) / 2, rndDist(eng), rndDist(eng));
        if (selectionHighlight_.get() == SelectionHighlight::FilterSelect &&
            selectionIn_.isSelected(getSelectionID(idx))) {
            colors[idx] = vec4(0.9f, 0.05f, 0.05f, 1);
        } else {
            baseCol = glm::normalize(baseCol);
            colors[idx] = vec4(baseCol.x, baseCol.y, baseCol.z, 1);
        }
    }
    mat4 modelMat = vortices->getModelMatrix();
    mat4 worldMat = vortices->getWorldMatrix();
    if (overrideScaleZ_) {
        // modelMat[2][2] = scaleZ_.get();
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

vec4 VortexToMesh::getTurningColor(Vortex::Turning rotation) {
    switch (rotation) {
        case Vortex::Turning::Clockwise:
            return vec4(1, 0, 0, 1);
        case Vortex::Turning::CounterClockwise:
            return vec4(0, 0, 1, 1);
        default:
            return vec4(0, 0, 0, 1);
    }
}

bool VortexToMesh::keepVortex(const Vortex& vort) const {
    switch (showSlices3D_.get()) {
        case ShowSlices3D::Height:
            return vort.timeSlice + 1 == timeSlice_.get();
        case ShowSlices3D::Time:
            return vort.heightSlice + 1 == heightSlice_.get();
        default:
            return vort.timeSlice + 1 == timeSlice_.get() &&
                   vort.heightSlice + 1 == heightSlice_.get();
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

size_t VortexToMesh::getSelectionID(size_t vortexID) const {
    // if (!dataFrameIn_.hasData() || ensembleMember_.get() <= 1) return vortexID;
    // size_t numRows = dataFrameIn_.getData()->getNumberOfRows();
    // size_t offsetID =
    //     dataFrameIn_.getData()->getNumberOfRows() - vorticesIn_.getData()->numGroups() +
    //     vortexID;
    // if (offsetID >= numRows) return vortexID;
    // return offsetID;
    // if (ensembleMember_.get() > 1) return return vortexID;
    return vortexID;
    // Column* colVortexId = nullptr;
    // Column* colEnsemble = nullptr;
    // for (auto col : *dataFrameIn_.getData()) {
    //     if (col->getHeader() == "Vortex ID") {
    //         colVortexId = col.get();
    //     }
    //     if (col->getHeader() == "Ensemble") {
    //         colEnsemble = col.get();
    //     }
    // }
    // if (!colVortexId || !colEnsemble) {
    //     LogWarn("Columns Ensemble and Vortex ID expected.");
    //     return false;
    // }

    // static bool firstID = false;
    // for (size_t idx = 0; idx < colVortexId->getSize(); ++idx) {
    //     if (static_cast<size_t>(colVortexId->getAsDouble(idx)) == vortexID &&
    //         static_cast<size_t>(colEnsemble->getAsDouble(idx)) == ensembleMember_)
    //         return idx;
    //     else if (firstID) {
    //         LogWarn("Ensemble " << colEnsemble->getAsDouble(idx)
    //                             << " - Vort: " << colVortexId->getAsDouble(idx));
    //     }
    // }
    // firstID = true;
    // LogWarn("Did not find eddy in DataFrame.");
    // return false;
}

vec4 VortexToMesh::getColor(bool firstVort, const vec4& groupColor, const Vortex& vort,
                            const dvec2& scoreRange) {
    vec4 color =
        (colorSource_.get() == ColorSource::Group) ? groupColor : getTurningColor(vort.rotation);
    if (!firstVort && groupDisplay_.get() == GroupDisplay::HighlightFirst) {
        color *= 0.5;
        color.a = 1;
    } else if (groupDisplay_.get() == GroupDisplay::Score) {
        color *= (vort.score - scoreRange.x) * 0.9 / (scoreRange.y - scoreRange.x) + 0.05;
        color.a = 1;
    }
    return color;
}

void VortexToMesh::createLineBoundaryMesh(const std::vector<vec4>& colors, const mat4& modelMat,
                                          const mat4& worldMat) {
    if (!vorticesIn_.hasData()) return;
    auto vortices = vorticesIn_.getData();
    // picking_.resize(vortices->numGroups() + 1);

    // Boundary mesh.
    std::vector<PickingMesh::Vertex> boundaryVerts;
    auto boundaryMesh = std::make_shared<PickingMesh>(DrawType::Lines, ConnectivityType::None);
    auto indexBufferBoundary =
        boundaryMesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None);
    auto& boundaryIndices = indexBufferBoundary->getDataContainer();
    boundaryMesh->setModelMatrix(modelMat);
    boundaryMesh->setWorldMatrix(worldMat);

    size_t numVerts = 0;
    size_t numGroups = skipLastGroup_.get() ? vortices->numGroups() - 1 : vortices->numGroups();
    for (size_t group = 0; group < numGroups; ++group) {
        if (singleGroupProperty_ && singleGroupProperty_.selectedGroup_.get() != group) continue;
        if (selectionHighlight_.get() == SelectionHighlight::FilterSelect &&
            selectionIn_.isFiltered(getSelectionID(group)))
            continue;
        bool firstVortex = true;
        size_t time = vortices->getTimeRange().y + 1;
        size_t height = vortices->getHeightRange().y + 1;
        for (auto vort = vortices->beginGroup(group); vort != vortices->endGroup(group); ++vort) {
            if (!keepVortex(*vort)) continue;
            vec4 color = getColor(firstVortex, colors[group], *vort, vortices->getScoreRange());
            if (groupDisplay_.get() == GroupDisplay::OnlyFirst && time == vort->timeSlice &&
                height == vort->heightSlice) {
                continue;
            } else if (groupDisplay_.get() == GroupDisplay::HighlightFirst &&
                       (time != vort->timeSlice || height != vort->heightSlice)) {
                color = getColor(true, colors[group], *vort, vortices->getScoreRange());
            }
            time = vort->timeSlice;
            height = vort->heightSlice;

            size_t valZ = getPos3D(*vort);
            for (const auto& point : vort->boundary) {
                boundaryVerts.push_back(
                    {vec3(point.x, point.y, valZ), color, picking_.getPickingId(group)});
            }

            for (size_t line = 1; line < vort->boundary.size(); ++line) {
                boundaryIndices.push_back(numVerts + line - 1);
                boundaryIndices.push_back(numVerts + line);
            }
            if (groupDisplay_.get() == GroupDisplay::HighlightFirst) {
                firstVortex = false;
            }
            numVerts += vort->size();
        }
    }
    boundaryMesh->addVertices(boundaryVerts);
    boundaryOut_.setData(boundaryMesh);
}

void VortexToMesh::createSurfaceBoundaryMesh(const std::vector<vec4>& colors, const mat4& modelMat,
                                             const mat4& worldMat) {
    if (!vorticesIn_.hasData() || vorticesIn_.getData()->size() == 0) return;
    auto vortices = vorticesIn_.getData();

    // Boundary mesh.
    std::vector<PickingMesh::Vertex> boundaryVerts;
    auto boundaryMesh = std::make_shared<PickingMesh>(DrawType::Triangles, ConnectivityType::None);
    auto indexBufferBoundary =
        boundaryMesh->addIndexBuffer(DrawType::Triangles, ConnectivityType::None);
    auto& boundaryIndices = indexBufferBoundary->getDataContainer();
    boundaryMesh->setModelMatrix(modelMat);
    boundaryMesh->setWorldMatrix(worldMat);

    size_t firstVertPreviousVort = 0;
    size_t firstVertCurrentLine = 0;
    size_t numGroups = skipLastGroup_.get() ? vortices->numGroups() - 1 : vortices->numGroups();
    for (size_t group = 0; group < numGroups; ++group) {
        if (singleGroupProperty_ && singleGroupProperty_.selectedGroup_.get() != group) continue;
        if (selectionHighlight_.get() == SelectionHighlight::FilterSelect &&
            selectionIn_.isFiltered(getSelectionID(group)))
            continue;
        const Vortex* previousVort = nullptr;
        std::vector<const Vortex*> orderedVorts;

        switch (showSlices3D_.get()) {
            case ShowSlices3D::Time:
                for (size_t time = 0; time < vortices->getTimeRange().y + 1; ++time) {
                    auto vortIt =
                        std::find_if(vortices->beginGroup(group), vortices->endGroup(group),
                                     [&](const Vortex& vort) {
                                         return vort.heightSlice + 1 == heightSlice_.get() &&
                                                vort.timeSlice == time;
                                     });
                    if (vortIt != vortices->endGroup(group)) orderedVorts.push_back(&*vortIt);
                }
                break;
            case ShowSlices3D::Height:
                for (size_t depth = 0; depth < vortices->getHeightRange().y + 1; ++depth) {
                    auto vortIt = std::find_if(vortices->beginGroup(group),
                                               vortices->endGroup(group), [&](const Vortex& vort) {
                                                   return vort.timeSlice + 1 == timeSlice_.get() &&
                                                          vort.heightSlice == depth;
                                               });
                    if (vortIt != vortices->endGroup(group)) orderedVorts.push_back(&*vortIt);
                }
                break;
            default:
                for (auto vort = vortices->beginGroup(group); vort != vortices->endGroup(group);
                     ++vort) {
                    if (keepVortex(*vort)) orderedVorts.push_back(&*vort);
                }
        }

        if (orderedVorts.size() == 0) continue;
        if (showSlices3D_.get() == ShowSlices3D::Height) {
            std::sort(
                orderedVorts.begin(), orderedVorts.end(),
                [](const Vortex* a, const Vortex* b) { return a->heightSlice < b->heightSlice; });
        } else if (showSlices3D_.get() == ShowSlices3D::Time) {
            std::sort(orderedVorts.begin(), orderedVorts.end(),
                      [](const Vortex* a, const Vortex* b) { return a->timeSlice < b->timeSlice; });
        }
        for (const Vortex* vort : orderedVorts) {
            size_t valZ = getPos3D(*vort);
            vec4 color =
                getColor(vort == orderedVorts[0], colors[group], *vort, vortices->getScoreRange());
            for (const auto& point : vort->boundary) {
                boundaryVerts.push_back(
                    {vec3(point.x, point.y, valZ), color, picking_.getPickingId(group)});
            }

            if (!previousVort || previousVort->rotation != vort->rotation) {
                firstVertPreviousVort = firstVertCurrentLine;
                firstVertCurrentLine = boundaryVerts.size();
                previousVort = &*vort;
                if (previousVort->rotation != vort->rotation) LogWarn("Flippedy!");
                continue;
            }
            // Connect to last line by strip.
            size_t initPrev =
                std::min_element(previousVort->boundary.begin(), previousVort->boundary.end(),
                                 [&](const dvec2& a, const dvec2& b) {
                                     return glm::distance2(a, vort->boundary[0]) <
                                            glm::distance2(b, vort->boundary[0]);
                                 }) -
                previousVort->boundary.begin();
            size_t currIdx = 0;
            size_t prevIdx = initPrev;
            size_t currNext = 1;
            size_t prevNext = (prevIdx + 1) % previousVort->size();
            double distCurrNext =
                glm::distance2(previousVort->boundary[prevIdx], vort->boundary[currNext]);
            double distPrevNext =
                glm::distance2(vort->boundary[currIdx], previousVort->boundary[prevNext]);

            do {
                if (previousVort->rotation == Vortex::Turning::Clockwise) {
                    boundaryIndices.push_back(firstVertPreviousVort + prevIdx);
                    boundaryIndices.push_back(firstVertCurrentLine + currIdx);
                } else {
                    boundaryIndices.push_back(firstVertCurrentLine + currIdx);
                    boundaryIndices.push_back(firstVertPreviousVort + prevIdx);
                }

                if (std::isinf(distCurrNext)) {
                    LogWarn("Curr is inf! Idx " << currNext << "/" << vort->size());
                    LogWarn(previousVort->boundary[prevIdx] << " -> " << vort->boundary[currNext]);
                    break;
                }
                if (std::isinf(distPrevNext)) {
                    LogWarn("Prev is inf! Idx " << prevIdx << "/" << previousVort->size());
                    LogWarn(vort->boundary[currIdx] << " -> " << previousVort->boundary[prevNext]);
                    break;
                }

                if (distCurrNext <= distPrevNext) {
                    boundaryIndices.push_back(firstVertCurrentLine + currNext);

                    currIdx = currNext;
                    if (currIdx == 0) {
                        distCurrNext = std::numeric_limits<double>::max();
                    } else {
                        currNext = (currIdx + 1) % vort->size();
                        distCurrNext = glm::distance2(previousVort->boundary[prevIdx],
                                                      vort->boundary[currNext]);
                    }
                } else {
                    boundaryIndices.push_back(firstVertPreviousVort + prevNext);

                    prevIdx = prevNext;
                    if (prevIdx == initPrev) {
                        distPrevNext = std::numeric_limits<double>::max();
                    } else {
                        prevNext = (prevIdx + 1) % previousVort->size();
                        distPrevNext = glm::distance2(vort->boundary[currIdx],
                                                      previousVort->boundary[prevNext]);
                    }
                }
            } while (currIdx != currNext || prevIdx != prevNext);
            firstVertPreviousVort = firstVertCurrentLine;
            firstVertCurrentLine = boundaryVerts.size();
            previousVort = &*vort;
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

    size_t numGroups = skipLastGroup_.get() ? vortices->numGroups() - 1 : vortices->numGroups();
    for (size_t group = 0; group < numGroups; ++group) {
        if (singleGroupProperty_ && singleGroupProperty_.selectedGroup_.get() != group) continue;
        size_t numVorts = 0;
        bool firstVortex = true;
        size_t time = vortices->getTimeRange().y + 1;
        size_t height = vortices->getHeightRange().y + 1;
        for (auto vort = vortices->beginGroup(group); vort != vortices->endGroup(group); ++vort) {
            if (!keepVortex(*vort)) continue;
            if (groupDisplay_.get() == GroupDisplay::OnlyFirst && time == vort->timeSlice &&
                height == vort->heightSlice) {
                continue;
            }
            time = vort->timeSlice;
            height = vort->heightSlice;
            numVorts++;
            size_t valZ = getPos3D(*vort);
            // for (const auto& point : vort->boundary) {
            centerPointVerts.push_back(
                {vec3(vort->center.x, vort->center.y, valZ), 0.002,
                 getColor(firstVortex, colors[group], *vort, vortices->getScoreRange())});
            firstVortex = false;
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

    size_t numGroups = skipLastGroup_.get() ? vortices->numGroups() - 1 : vortices->numGroups();
    for (size_t group = 0; group < numGroups; ++group) {
        if (singleGroupProperty_ && singleGroupProperty_.selectedGroup_.get() != group) continue;
        bool firstVortex = true;
        size_t time = vortices->getTimeRange().y + 1;
        size_t height = vortices->getHeightRange().y + 1;
        for (auto vort = vortices->beginGroup(group); vort != vortices->endGroup(group); ++vort) {
            if (!keepVortex(*vort)) continue;
            if (groupDisplay_.get() == GroupDisplay::OnlyFirst && time == vort->timeSlice &&
                height == vort->heightSlice) {
                continue;
            }
            time = vort->timeSlice;
            height = vort->heightSlice;

            size_t valZ = getPos3D(*vort);
            centerPointVerts.push_back(
                {vec3(vort->center.x, vort->center.y, valZ),
                 getColor(true, colors[group], *vort, vortices->getScoreRange())});
            if (!firstVortex) {
                centerIndices.push_back(centerPointVerts.size());
                centerIndices.push_back(centerPointVerts.size() + 1);
            }
            firstVortex = false;
        }
    }
    centerMesh->addVertices(centerPointVerts);
    centerOut_.setData(centerMesh);
}

void VortexToMesh::objectPicked(PickingEvent* p) {
    if (p->getState() == PickingState::Updated && p->getPressState() == PickingPressState::Press &&
        p->getPressItem() == PickingPressItem::Primary) {
        selectionIn_.sendSelectionEvent({getSelectionID(p->getPickedId())});
    }
}

VortexToMesh::SingleGroupProperty::SingleGroupProperty(const std::string& identifier,
                                                       const std::string& displayName)
    : BoolCompositeProperty(identifier, displayName, false)
    , selectedGroup_("selectedGroup", "Group", 0, 0, 1) {
    addProperty(selectedGroup_);
}

}  // namespace inviwo
