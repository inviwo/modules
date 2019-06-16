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

#include <inviwo/topologytoolkit/ports/morsesmalecomplexport.h>

namespace inviwo {

namespace topology {

void MorseSmaleComplexData::setMSCOutput(ttk::MorseSmaleComplex &msc, size_t numberOfVertices) {
    msc.setOutputCriticalPoints(
        &criticalPoints.numberOfPoints, &criticalPoints.points,
        &criticalPoints.points_cellDimensions, &criticalPoints.points_cellIds,
        &criticalPoints.points_cellScalars, &criticalPoints.points_isOnBoundary,
        &criticalPoints.points_PLVertexIdentifiers, &criticalPoints.points_manifoldSize);
    msc.setOutputSeparatrices1(
        &separatrices.numberOfPoints, &separatrices.points, &separatrices.points_smoothingMask,
        &separatrices.points_cellDimensions, &separatrices.points_cellIds,
        &separatrices.numberOfCells, &separatrices.cells, &separatrices.cells_sourceIds,
        &separatrices.cells_destinationIds, &separatrices.cells_separatrixIds,
        &separatrices.cells_separatrixTypes, &separatrices.cells_separatrixFunctionMaxima,
        &separatrices.cells_separatrixFunctionMinima, &separatrices.cells_separatrixFunctionDiffs,
        &separatrices.cells_isOnBoundary);

    segmentation.ascending = std::vector(numberOfVertices, -1);
    segmentation.descending = std::vector(numberOfVertices, -1);
    segmentation.msc = std::vector(numberOfVertices, -1);

    msc.setOutputMorseComplexes(segmentation.ascending.data(), segmentation.descending.data(),
                                segmentation.msc.data());
}

}  // namespace topology

}  // namespace inviwo
