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
#include <inviwo/core/datastructures/datatraits.h>
#include <inviwo/core/util/document.h>
#include <inviwo/core/datastructures/buffer/buffer.h>

#include <inviwo/topologytoolkit/datastructures/triangulationdata.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/morseSmaleComplex/MorseSmaleComplex.h>
#include <warn/pop>

#include <vector>

namespace inviwo {

namespace topology {

struct IVW_MODULE_TOPOLOGYTOOLKIT_API MorseSmaleComplexData {

    /**
     * set the various outputs of the ttk Morse-Smale complex \p msc to the arrays in this struct.
     * The buffers will have the same data format as the scalar values stored in the triangulation.
     *
     * @param msc   the ttk Morse-Smale complex object whose output is set up
     * @param t     matching triangulation for which the Morse-Smale complex is computed
     */
    MorseSmaleComplexData(ttk::MorseSmaleComplex& msc, std::shared_ptr<const TriangulationData> t);

    // critical points
    struct CriticalPoints {
        ttk::SimplexId numberOfPoints = 0;
        /**
         * Stores points linearly, i.e. in 3D the first 3 values is point 1
         * Size is should be dimension times numberOfPoints
         */
        std::vector<float> points;
        std::vector<char> cellDimensions;
        std::vector<ttk::SimplexId> cellIds;
        std::vector<char> isOnBoundary;
        std::vector<ttk::SimplexId> PLVertexIdentifiers;
        std::vector<ttk::SimplexId> manifoldSize;
        std::shared_ptr<BufferBase> scalars;
    };
    // 1-separatrices
    struct SeparatricesPoints {
        ttk::SimplexId numberOfPoints = 0;
        std::vector<float> points;
        std::vector<char> smoothingMask;
        std::vector<char> cellDimensions;
        std::vector<ttk::SimplexId> cellIds;
    };
    struct SeparatrixCells {
        ttk::SimplexId numberOfCells = 0;
        std::vector<ttk::SimplexId> cells;
        std::vector<ttk::SimplexId> sourceIds;
        std::vector<ttk::SimplexId> destinationIds;
        std::vector<ttk::SimplexId> separatrixIds;
        std::vector<char> types;
        std::vector<char> isOnBoundary;
        std::shared_ptr<BufferBase> functionMaxima;
        std::shared_ptr<BufferBase> functionMinima;
        std::shared_ptr<BufferBase> functionDiffs;
    };
    // segmentation
    struct Segmentation {
        std::vector<ttk::SimplexId> ascending;
        std::vector<ttk::SimplexId> descending;
        std::vector<ttk::SimplexId> msc;
    };

    CriticalPoints criticalPoints;
    SeparatricesPoints separatrixPoints;
    SeparatrixCells separatrixCells;
    Segmentation segmentation;

    std::shared_ptr<const TriangulationData> triangulation;
};

}  // namespace topology

template <>
struct DataTraits<topology::MorseSmaleComplexData> {
    static std::string classIdentifier() { return "org.inviwo.topology.morsesmalecomplexdata"; }
    static std::string dataName() { return "TTK Morse-Smale Complex"; }
    static uvec3 colorCode() { return uvec3(105, 152, 188); }
    static Document info(const topology::MorseSmaleComplexData& data) {
        using H = utildoc::TableBuilder::Header;
        using P = Document::PathComponent;
        Document doc;
        doc.append("b", dataName(), {{"style", "color:white;"}});
        utildoc::TableBuilder tb(doc.handle(), P::end());
        if (data.triangulation) {
            tb(H("Dimensionality"), data.triangulation->getTriangulation().getDimensionality());
        }
        tb(H("Critical Points"), data.criticalPoints.numberOfPoints);
        tb(H("Separatrices No. Points"), data.separatrixPoints.numberOfPoints);
        tb(H("Separatrices No. Cells"), data.separatrixCells.numberOfCells);
        return doc;
    }
};

}  // namespace inviwo
