/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2019 Inviwo Foundation
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

#include <inviwo/topologytoolkit/processors/ttktestprocessor.h>
#include <inviwo/topologytoolkit/utils/ttkutils.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>
#include <inviwo/core/util/filesystem.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/morseSmaleComplex/MorseSmaleComplex.h>
#include <ttk/core/base/persistenceCurve/PersistenceCurve.h>
#include <ttk/core/base/persistenceDiagram/PersistenceDiagram.h>
#include <ttk/core/base/topologicalSimplification/TopologicalSimplification.h>
#include <warn/pop>

#include <fstream>
#include <sstream>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TTKTestProcessor::processorInfo_{
    "org.inviwo.ttk.TTKTestProcessor",  // Class identifier
    "TTKTest Processor",                // Display name
    "Topology",                         // Category
    CodeState::Experimental,            // Code state
    "CPU, TTK, Testing",                // Tags
};
const ProcessorInfo TTKTestProcessor::getProcessorInfo() const { return processorInfo_; }

TTKTestProcessor::TTKTestProcessor()
    : Processor()
    , inport_("mesh")
    , outport_("outport")
    , color_("color", "Mesh Color", vec4(1.0f), vec4(0.0f), vec4(1.0f))
    , threshold_("threshold", "Threshold", 0.05f, 0.0f, 10.0f) {

    addPort(inport_);
    addPort(outport_);

    color_.setSemantics(PropertySemantics::Color);

    addProperty(color_);
    addProperty(threshold_);

    isSink_.setUpdate([]() { return true; });
}

void TTKTestProcessor::process() {
    // convert input mesh to TTK triangle data
    auto ttkData = topology::meshToTTKTriangulation(*inport_.getData().get());

    if (ttkData.getPoints().empty()) {
        outport_.setData(std::make_shared<inviwo::Mesh>());
        return;
    }

    // ttk::welcomeMsg_ = true;
    // ttk::goodbyeMsg_ = true;
    // ttk::globalDebugLevel_ = 3;

    // NOW, do the TTK processing

    LogInfo("1. computing some elevation");
    std::vector<float> height(ttkData.getPoints().size());
    std::vector<int> offsets(height.size());
    // use the z-coordinate here
    for (size_t i = 0; i < ttkData.getPoints().size(); ++i) {
        height[i] = ttkData[i].z;
        offsets[i] = static_cast<int>(i);
    }

    LogInfo("2. computing the persistence curve");
    ttk::PersistenceCurve curve;
    std::vector<std::pair<float, ttk::SimplexId> > outputCurve;
    curve.setupTriangulation(&ttkData.getTriangulation());
    curve.setInputScalars(height.data());
    curve.setInputOffsets(offsets.data());
    curve.setOutputCTPlot(&outputCurve);
    curve.execute<float, int>();

    LogInfo("  ttk::PersistenceCurve has " << outputCurve.size() << " entries.");

    LogInfo("3. computing the persistence diagram");
    ttk::PersistenceDiagram diagram;
    std::vector<std::tuple<ttk::SimplexId, ttk::CriticalType, ttk::SimplexId, ttk::CriticalType,
                           float, ttk::SimplexId> >
        diagramOutput;
    diagram.setupTriangulation(&ttkData.getTriangulation());
    diagram.setInputScalars(height.data());
    diagram.setInputOffsets(offsets.data());
    diagram.setOutputCTDiagram(&diagramOutput);
    diagram.execute<float, int>();

    LogInfo("  ttk::PersistenceDiagram has " << diagramOutput.size() << " entries.");

    LogInfo("4. selecting the critical point pairs");
    std::vector<float> simplifiedHeight = height;
    std::vector<int> authorizedCriticalPoints, simplifiedOffsets = offsets;
    for (size_t i = 0; i < diagramOutput.size(); i++) {
        double persistence = std::get<4>(diagramOutput[i]);
        if (persistence > threshold_.get()) {
            // 5. selecting the most persistent pairs
            authorizedCriticalPoints.push_back(std::get<0>(diagramOutput[i]));
            authorizedCriticalPoints.push_back(std::get<2>(diagramOutput[i]));
        }
    }

    // determine max persistence for threshold
    auto maxIt = std::max_element(
        diagramOutput.begin(), diagramOutput.end(),
        [](const auto &a, const auto &b) { return std::get<4>(a) < std::get<4>(b); });
    if (maxIt != diagramOutput.end()) {
        threshold_.setMaxValue(std::get<4>(*maxIt));
    }

    LogInfo("6. simplifying the input data to remove non-persistent pairs");
    ttk::TopologicalSimplification simplification;
    simplification.setupTriangulation(&ttkData.getTriangulation());
    simplification.setInputScalarFieldPointer(height.data());
    simplification.setInputOffsetScalarFieldPointer(offsets.data());
    simplification.setOutputOffsetScalarFieldPointer(simplifiedOffsets.data());
    simplification.setOutputScalarFieldPointer(simplifiedHeight.data());
    simplification.setConstraintNumber(static_cast<int>(authorizedCriticalPoints.size()));
    simplification.setVertexIdentifierScalarFieldPointer(authorizedCriticalPoints.data());
    simplification.execute<float, int>();

    // assign the simplified values to the input mesh
    for (size_t i = 0; i < simplifiedHeight.size(); i++) {
        ttkData[i].z = simplifiedHeight[i];
    }

    LogInfo("7. computing the Morse-Smale complex");
    ttk::MorseSmaleComplex morseSmaleComplex;
    // critical points
    int criticalPoints_numberOfPoints;
    std::vector<float> criticalPoints_points;
    std::vector<char> criticalPoints_points_cellDimensions;
    std::vector<int> criticalPoints_points_cellIds;
    std::vector<char> criticalPoints_points_isOnBoundary;
    std::vector<float> criticalPoints_points_cellScalars;
    std::vector<int> criticalPoints_points_PLVertexIdentifiers;
    std::vector<int> criticalPoints_points_manifoldSize;
    // 1-separatrices
    int separatrices1_numberOfPoints;
    std::vector<float> separatrices1_points;
    std::vector<char> separatrices1_points_smoothingMask;
    std::vector<char> separatrices1_points_cellDimensions;
    std::vector<int> separatrices1_points_cellIds;
    int separatrices1_numberOfCells{};
    std::vector<int> separatrices1_cells;
    std::vector<int> separatrices1_cells_sourceIds;
    std::vector<int> separatrices1_cells_destinationIds;
    std::vector<int> separatrices1_cells_separatrixIds;
    std::vector<char> separatrices1_cells_separatrixTypes;
    std::vector<char> separatrices1_cells_isOnBoundary;
    std::vector<float> separatrices1_cells_separatrixFunctionMaxima;
    std::vector<float> separatrices1_cells_separatrixFunctionMinima;
    std::vector<float> separatrices1_cells_separatrixFunctionDiffs;
    // segmentation
    std::vector<int> ascendingSegmentation(ttkData.getTriangulation().getNumberOfVertices(), -1),
        descendingSegmentation(ttkData.getTriangulation().getNumberOfVertices(), -1),
        mscSegmentation(ttkData.getTriangulation().getNumberOfVertices(), -1);
    morseSmaleComplex.setupTriangulation(&ttkData.getTriangulation());
    morseSmaleComplex.setInputScalarField(simplifiedHeight.data());
    morseSmaleComplex.setInputOffsets(simplifiedOffsets.data());
    morseSmaleComplex.setOutputMorseComplexes(
        ascendingSegmentation.data(), descendingSegmentation.data(), mscSegmentation.data());
    morseSmaleComplex.setOutputCriticalPoints(
        &criticalPoints_numberOfPoints, &criticalPoints_points,
        &criticalPoints_points_cellDimensions, &criticalPoints_points_cellIds,
        &criticalPoints_points_cellScalars, &criticalPoints_points_isOnBoundary,
        &criticalPoints_points_PLVertexIdentifiers, &criticalPoints_points_manifoldSize);
    morseSmaleComplex.setOutputSeparatrices1(
        &separatrices1_numberOfPoints, &separatrices1_points, &separatrices1_points_smoothingMask,
        &separatrices1_points_cellDimensions, &separatrices1_points_cellIds,
        &separatrices1_numberOfCells, &separatrices1_cells, &separatrices1_cells_sourceIds,
        &separatrices1_cells_destinationIds, &separatrices1_cells_separatrixIds,
        &separatrices1_cells_separatrixTypes, &separatrices1_cells_separatrixFunctionMaxima,
        &separatrices1_cells_separatrixFunctionMinima, &separatrices1_cells_separatrixFunctionDiffs,
        &separatrices1_cells_isOnBoundary);

    morseSmaleComplex.execute<float, int>();
    LogInfo("Done.");

    // set output mesh
    outport_.setData(topology::ttkTriangulationToMesh(ttkData, color_.get()));
}

}  // namespace inviwo
