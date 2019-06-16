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
#pragma once

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>

#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/ports/dataoutport.h>

#include <inviwo/topologytoolkit/datastructures/triangulationdata.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/morseSmaleComplex/MorseSmaleComplex.h>
#include <warn/pop>

#include <vector>

namespace inviwo {

namespace topology {

struct IVW_MODULE_TOPOLOGYTOOLKIT_API MorseSmaleComplexData {
        
    // critical points
    struct CriticalPoints {
        int numberOfPoints;
        std::vector<float> points;
        std::vector<char> points_cellDimensions;
        std::vector<int> points_cellIds;
        std::vector<char> points_isOnBoundary;
        std::vector<float> points_cellScalars;
        std::vector<int> points_PLVertexIdentifiers;
        std::vector<int> points_manifoldSize;
    };
    // 1-separatrices
    struct Separatrices {
        int numberOfPoints;
        std::vector<float> points;
        std::vector<char> points_smoothingMask;
        std::vector<char> points_cellDimensions;
        std::vector<int> points_cellIds;
        int numberOfCells{};
        std::vector<int> cells;
        std::vector<int> cells_sourceIds;
        std::vector<int> cells_destinationIds;
        std::vector<int> cells_separatrixIds;
        std::vector<char> cells_separatrixTypes;
        std::vector<char> cells_isOnBoundary;
        std::vector<float> cells_separatrixFunctionMaxima;
        std::vector<float> cells_separatrixFunctionMinima;
        std::vector<float> cells_separatrixFunctionDiffs;
    };

    void setMSCOutput(ttk::MorseSmaleComplex &msc);

    CriticalPoints criticalPoints;
        Separatrices separatrices;

    std::shared_ptr<const TriangulationData> triangulation;
};

/**
 * \ingroup ports
 */
using MorseSmaleComplexInport = DataInport<MorseSmaleComplexData>;
/**
 * \ingroup ports
 */
using MorseSmaleComplexOutport = DataOutport<MorseSmaleComplexData>;

}  // namespace topology

template <>
struct DataTraits<topology::MorseSmaleComplexData> {
    static std::string classIdentifier() { return "org.inviwo.topology.morsesmalecomplexdata"; }
    static std::string dataName() { return "TTK Morse-Smale Complex"; }
    static uvec3 colorCode() { return uvec3(94, 152, 224); }
    static Document info(const topology::MorseSmaleComplexData& data) {
        using H = utildoc::TableBuilder::Header;
        using P = Document::PathComponent;
        Document doc;
        doc.append("b", dataName(), {{"style", "color:white;"}});
        utildoc::TableBuilder tb(doc.handle(), P::end());
        tb(H("Critical Points"), data.criticalPoints.numberOfPoints);
        tb(H("Separatrices No. Points"), data.separatrices.numberOfPoints);
        tb(H("Separatrices No. Cells"), data.separatrices.numberOfCells);
        return doc;
    }
};

}  // namespace inviwo
