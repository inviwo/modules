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

#include <inviwo/topologytoolkit/datastructures/morsesmalecomplexdata.h>
#include <inviwo/core/datastructures/buffer/bufferramprecision.h>
#include <inviwo/core/util/assertion.h>

namespace inviwo {

namespace topology {

MorseSmaleComplexData::MorseSmaleComplexData(ttk::MorseSmaleComplex& msc,
                                             std::shared_ptr<const TriangulationData> t)
    : triangulation(t) {

    IVW_ASSERT(triangulation, "triangulation is not valid");

    t->getScalarValues()
        ->getRepresentation<BufferRAM>()
        ->dispatch<void, dispatching::filter::Scalars>([this, &msc](const auto buffer) {
            using ValueType = util::PrecisionValueType<decltype(buffer)>;

            auto cellScalarsRAM = std::make_shared<BufferRAMPrecision<ValueType>>();
            auto functionMaxRAM = std::make_shared<BufferRAMPrecision<ValueType>>();
            auto functionMinRAM = std::make_shared<BufferRAMPrecision<ValueType>>();
            auto functionDiffRAM = std::make_shared<BufferRAMPrecision<ValueType>>();

            msc.setOutputCriticalPoints(
                &criticalPoints.numberOfPoints, &criticalPoints.points,
                &criticalPoints.cellDimensions, &criticalPoints.cellIds,
                &cellScalarsRAM->getDataContainer(), &criticalPoints.isOnBoundary,
                &criticalPoints.PLVertexIdentifiers, &criticalPoints.manifoldSize);
            msc.setOutputSeparatrices1(
                &separatrixPoints.numberOfPoints, &separatrixPoints.points,
                &separatrixPoints.smoothingMask, &separatrixPoints.cellDimensions,
                &separatrixPoints.cellIds, &separatrixCells.numberOfCells, &separatrixCells.cells,
                &separatrixCells.sourceIds, &separatrixCells.destinationIds,
                &separatrixCells.separatrixIds, &separatrixCells.types,
                &functionMaxRAM->getDataContainer(), &functionMinRAM->getDataContainer(),
                &functionDiffRAM->getDataContainer(), &separatrixCells.isOnBoundary);

            criticalPoints.scalars = std::make_shared<Buffer<ValueType>>(cellScalarsRAM);
            separatrixCells.functionMaxima = std::make_shared<Buffer<ValueType>>(functionMaxRAM);
            separatrixCells.functionMinima = std::make_shared<Buffer<ValueType>>(functionMinRAM);
            separatrixCells.functionDiffs = std::make_shared<Buffer<ValueType>>(functionDiffRAM);
        });

    const auto numVertices = t->getPoints().size();
    segmentation.ascending = std::vector(numVertices, -1);
    segmentation.descending = std::vector(numVertices, -1);
    segmentation.msc = std::vector(numVertices, -1);

    msc.setOutputMorseComplexes(segmentation.ascending.data(), segmentation.descending.data(),
                                segmentation.msc.data());
}

}  // namespace topology

}  // namespace inviwo
