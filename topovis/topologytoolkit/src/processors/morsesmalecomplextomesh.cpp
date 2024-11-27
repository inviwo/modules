/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2024 Inviwo Foundation
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
#include <inviwo/core/interaction/events/pickingevent.h>

#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/document.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>

#include <algorithm>
#include <fmt/format.h>

namespace inviwo {

namespace {

template <bool PBC>
std::shared_ptr<Mesh> MSCToMesh(const topology::MorseSmaleComplexData& msc,
                                const TopologyColorsProperty& colorProp,
                                const TopologyFilterProperty& filterProp, float sphereRadius,
                                float lineThickness, PickingMapper& pickingExtrema,
                                PickingMapper& pickingSeperatrix) {
    // Prepare Memory
    std::vector<vec3> positions;
    std::vector<vec4> colors;
    std::vector<float> radius;
    std::vector<uint32_t> picking;

    const auto& trig = msc.triangulation->getTriangulation();
    const auto numcp = msc.criticalPoints.numberOfPoints;
    const auto dimensionality = trig.getDimensionality();
    const auto ext = msc.triangulation->getGridExtent();

    std::unordered_map<ttk::SimplexId, char> idToDims;

    // Add critical points with their color
    pickingExtrema.resize(numcp);
    const auto& cpoints = msc.criticalPoints.points;
    std::vector<uint32_t> cpIndices;
    for (ttk::SimplexId i = 0; i < numcp; i++) {
        const auto cellDim = msc.criticalPoints.cellDimensions[i];
        idToDims[msc.criticalPoints.cellIds[i]] = cellDim;

        if (!filterProp.showExtrema(dimensionality, cellDim)) continue;

        positions.emplace_back(cpoints[i * 3 + 0], cpoints[i * 3 + 1], cpoints[i * 3 + 2]);
        colors.emplace_back(colorProp.getColor(dimensionality, cellDim));
        radius.emplace_back(sphereRadius);
        picking.emplace_back(pickingExtrema.getPickingId(i));
        cpIndices.emplace_back(positions.size() - 1);
    }

    // Add the separatrixCells
    pickingSeperatrix.resize(msc.separatrixCells.numberOfCells);
    const auto& spoints = msc.separatrixPoints.points;
    std::vector<uint32_t> sepIndices;

    for (ttk::SimplexId i = 0; i < msc.separatrixCells.numberOfCells; ++i) {
        if (!filterProp.showSeperatrix(dimensionality, msc.separatrixCells.types[i])) continue;

        const auto src = msc.separatrixCells.cells[3 * i + 1];
        const auto dst = msc.separatrixCells.cells[3 * i + 2];

        std::array<vec3, 2> points{
            {{spoints[3 * src + 0], spoints[3 * src + 1], spoints[3 * src + 2]},
             {spoints[3 * dst + 0], spoints[3 * dst + 1], spoints[3 * dst + 2]}}};

        if constexpr (PBC) {
            points[0] += vec3{glm::lessThan(points[0] - points[1], -0.5f * ext)} * ext;
            points[1] += vec3{glm::greaterThan(points[0] - points[1], 0.5f * ext)} * ext;
        }

        for (const auto& point : points) {
            positions.emplace_back(point);
            colors.emplace_back(*colorProp.arc_);
            radius.emplace_back(lineThickness);
            picking.emplace_back(pickingSeperatrix.getPickingId(i));
            sepIndices.emplace_back(positions.size() - 1);
        }
    }

    auto mesh = std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    mesh->addBuffer(BufferType::PositionAttrib, util::makeBuffer(std::move(positions)));
    mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(std::move(colors)));
    mesh->addBuffer(BufferType::RadiiAttrib, util::makeBuffer(std::move(radius)));
    mesh->addBuffer(BufferType::PickingAttrib, util::makeBuffer(std::move(picking)));

    mesh->addIndices(Mesh::MeshInfo(DrawType::Points, ConnectivityType::None),
                     util::makeIndexBuffer(std::move(cpIndices)));
    mesh->addIndices(Mesh::MeshInfo(DrawType::Lines, ConnectivityType::None),
                     util::makeIndexBuffer(std::move(sepIndices)));

    // vertex positions are already transformed
    mesh->setModelMatrix(mat4(1.0f));
    mesh->setWorldMatrix(msc.triangulation->getWorldMatrix());
    mesh->copyMetaDataFrom(*msc.triangulation);

    return mesh;
}

}  // namespace

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MorseSmaleComplexToMesh::processorInfo_{
    "org.inviwo.MorseSmaleComplexToMesh",  // Class identifier
    "Morse-Smale Complex To Mesh",         // Display name
    "Topology",                            // Category
    CodeState::Experimental,               // Code state
    "CPU, Topology, TTK, Contour Tree",    // Tags
};
const ProcessorInfo& MorseSmaleComplexToMesh::getProcessorInfo() const { return processorInfo_; }

MorseSmaleComplexToMesh::MorseSmaleComplexToMesh()
    : Processor()
    , colors_("colors", "Colors")
    , filters_("filters", "Filters")
    , sphereRadius_("sphereRadius", "Extrema Radius", 0.05f, 0.0f, 10.0f)
    , lineThickness_("lineThickness", "Seperatrix Thickness", 0.025f, 0.0f, 10.0f)
    , pickingExtrema_{this, 1, [this](PickingEvent* e) { handleExtremaPicking(e); }}
    , pickingSeperatrix_{this, 1, [this](PickingEvent* e) { handleSeperatrixPicking(e); }} {

    addPort(mscInport_);
    addPort(outport_);

    addProperties(colors_, sphereRadius_, lineThickness_, filters_);
}

void MorseSmaleComplexToMesh::process() {
    auto msc = mscInport_.getData();

    if (msc->triangulation->getTriangulation().usesPeriodicBoundaryConditions()) {
        outport_.setData(MSCToMesh<true>(*msc, colors_, filters_, *sphereRadius_, *lineThickness_,
                                         pickingExtrema_, pickingSeperatrix_));
    } else {
        outport_.setData(MSCToMesh<false>(*msc, colors_, filters_, *sphereRadius_, *lineThickness_,
                                          pickingExtrema_, pickingSeperatrix_));
    }
}

void MorseSmaleComplexToMesh::handleExtremaPicking(PickingEvent* p) {
    if (p->getHoverState() == PickingHoverState::Move && p->getPressItems().empty()) {

        if (auto msc = mscInport_.getData()) {
            const auto id = p->getPickedId();
            const auto& cp = msc->criticalPoints;

            Document doc;
            using P = Document::PathComponent;
            auto b = doc.append("html").append("body");

            b.append("b", "Extrema", {{"style", "color:white;"}});
            using H = utildoc::TableBuilder::Header;

            utildoc::TableBuilder tb(b, P::end());
            tb(H("Id"), id);
            tb(H("Value"), cp.scalars->getRepresentation<BufferRAM>()->getAsDouble(id));
            tb(H("Position"),
               vec3{cp.points[3 * id + 0], cp.points[3 * id + 1], cp.points[3 * id + 2]});
            tb(H("Cell Dim"), int{cp.cellDimensions[id]});
            tb(H("Cell Id"), cp.cellIds[id]);
            tb(H("On Boundary"), cp.isOnBoundary[id] ? "True" : "False");

            p->setToolTip(doc.str());

        } else {
            p->setToolTip(fmt::format("Extrema: {}", p->getPickedId()));
        }
    } else if (p->getHoverState() == PickingHoverState::Exit) {
        p->setToolTip("");
    }
}

void MorseSmaleComplexToMesh::handleSeperatrixPicking(PickingEvent* p) {
    if (p->getHoverState() == PickingHoverState::Move && p->getPressItems().empty()) {

        if (auto msc = mscInport_.getData()) {

            const auto i = p->getPickedId();

            const auto id = msc->separatrixCells.separatrixIds[i];
            const auto srcInd = msc->separatrixCells.cells[3 * i + 1];
            const auto dstInd = msc->separatrixCells.cells[3 * i + 2];

            const int srcDim = msc->separatrixPoints.cellDimensions[srcInd];
            const int srcId = msc->separatrixPoints.cellIds[srcInd];

            const int dstDim = msc->separatrixPoints.cellDimensions[dstInd];
            const int dstId = msc->separatrixPoints.cellIds[dstInd];

            const int srcCpCellId = msc->separatrixCells.sourceIds[i];
            const int dstCpCellId = msc->separatrixCells.destinationIds[i];

            const auto cbegin = msc->criticalPoints.cellIds.begin();
            const auto cend = msc->criticalPoints.cellIds.end();
            const auto& cpdims = msc->criticalPoints.cellDimensions;
            const auto srcIt = std::find(cbegin, cend, srcCpCellId);
            const auto dstIt = std::find(cbegin, cend, dstCpCellId);

            Document doc;
            using P = Document::PathComponent;
            auto b = doc.append("html").append("body");
            b.append("b", "Seperatrix", {{"style", "color:white;"}});
            using H = utildoc::TableBuilder::Header;
            utildoc::TableBuilder tb(b, P::end());

            tb(H("Id"), id, "");
            tb(H("Type"), int{msc->separatrixCells.types[i]}, "");
            tb(H("On Boundary"), msc->separatrixCells.isOnBoundary[i] ? "True" : "False", "");

            tb("", H("Source"), H("Destination"));
            tb(H("Id"), srcId, dstId);
            tb(H("Dim"), srcDim, dstDim);
            tb(H("CP Cell"), srcCpCellId, dstCpCellId);
            tb(H("CP Id"), srcIt != cend ? std::distance(cbegin, srcIt) : -1,
               dstIt != cend ? std::distance(cbegin, dstIt) : -1);
            tb(H("CP Dim"), srcIt != cend ? int{cpdims[std::distance(cbegin, srcIt)]} : -1,
               dstIt != cend ? int{cpdims[std::distance(cbegin, dstIt)]} : -1);

            tb(H("x"), msc->separatrixPoints.points[3 * srcInd + 0],
               msc->separatrixPoints.points[3 * dstInd + 0]);
            tb(H("y"), msc->separatrixPoints.points[3 * srcInd + 1],
               msc->separatrixPoints.points[3 * dstInd + 1]);
            tb(H("z"), msc->separatrixPoints.points[3 * srcInd + 2],
               msc->separatrixPoints.points[3 * dstInd + 2]);

            p->setToolTip(doc.str());

        } else {
            p->setToolTip("Seperatrix: " + toString(p->getPickedId()));
        }

    } else if (p->getHoverState() == PickingHoverState::Exit) {
        p->setToolTip("");
    }
}

}  // namespace inviwo
