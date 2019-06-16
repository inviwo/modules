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

#include <inviwo/topologytoolkit/processors/morsesmalecomplex.h>
#include <inviwo/topologytoolkit/utils/ttkutils.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/geometry/meshram.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/morseSmaleComplex/MorseSmaleComplex.h>
#include <ttk/core/base/topologicalSimplification/TopologicalSimplification.h>
#include <ttk/core/base/ftmTree/FTMTree_MT.h>
#include <warn/pop>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MorseSmaleComplex::processorInfo_{
    "org.inviwo.MorseSmaleComplex",      // Class identifier
    "Morse-Smale Complex",               // Display name
    "Topology",                          // Category
    CodeState::Experimental,             // Code state
    "CPU, Topology, TTK, Contour Tree",  // Tags
};
const ProcessorInfo MorseSmaleComplex::getProcessorInfo() const { return processorInfo_; }

MorseSmaleComplex::MorseSmaleComplex() : Processor(), separationSurfaces_{"separationSurfaces", "Separation Surfaces", true } {

    addPort(inport_);
    addPort(outport_);
    addProperty(separationSurfaces_);
}

void MorseSmaleComplex::process() {

    // construction of ttk contour tree
    auto computeTree = [this, inportData, threadCount, treeType, segmentation,
                        normalization](const auto buffer) {
        using ValueType = util::PrecisionValueType<decltype(buffer)>;
        using PrimitiveType = typename DataFormat<ValueType>::primitive;

        std::vector<int> offsets(inportData->getOffsets());

        auto tree = std::make_shared<topology::ContourTree>();

        tree->setThreadNumber(threadCount);
        tree->setupTriangulation(const_cast<ttk::Triangulation *>(&inportData->getTriangulation()));
        // tree->setDebugLevel(0);
        tree->setVertexScalars(buffer->getDataContainer().data());
        tree->setVertexSoSoffsets(offsets.data());
        tree->setTreeType(static_cast<int>(treeType));
        tree->setSegmentation(segmentation);
        tree->setNormalizeIds(normalization);

        tree->build<PrimitiveType, int>();
        return tree;
    };

    dispatchPool([this, inportData = inport_.getData(),
                  computeTree]() {
        auto treeData = std::make_shared<topology::ContourTreeData>();
        treeData->type = treeType;
        treeData->triangulation = inportData;

        treeData->tree =
            inportData->getScalarValues()
                ->getRepresentation<BufferRAM>()
                ->dispatch<std::shared_ptr<topology::ContourTree>, dispatching::filter::Scalars>(
                    computeTree);

        dispatchFront([this, treeData]() {
            treeData_ = treeData;
            invalidate(InvalidationLevel::InvalidOutput);
        });

        treeIsFinished_ = true;
    });
    }

    outport_.setData(treeData_);



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


}

}  // namespace inviwo
