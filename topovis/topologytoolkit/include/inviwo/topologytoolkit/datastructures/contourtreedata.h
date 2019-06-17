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
#include <inviwo/topologytoolkit/datastructures/triangulationdata.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/ftmTree/FTMTree.h>
#include <ttk/core/base/ftmTree/FTMTree_MT.h>
#include <warn/pop>

#include <ostream>

namespace inviwo {

namespace topology {

// CHECK: JoinAndSplit might be deprecated in ttk
enum class TreeType { Join = 0, Split = 1, Contour = 2, JoinAndSplit = 3 };

using ContourTree = ttk::ftm::FTMTree;

struct IVW_MODULE_TOPOLOGYTOOLKIT_API ContourTreeData {
    // FIXME: Storing the tree type explicitly is necessary due to a TTK bug!
    //
    // FTMTree stores the type only as parameter which renders FTMTree::isJT() and FTMTree::isST()
    // useless (the tree object is always a 'Contour tree'). Trying to call
    // tree->getNumberOfSuperArcs() of a join tree will result in a nullptr exception as it will
    // access the data arrays of the contour tree and not the join tree.
    // There is also no way to access the parameters of the FTMTree.
    TreeType type;
    ttk::ftm::FTMTree_MT* getTree() const;

    std::shared_ptr<ttk::ftm::FTMTree> tree;
    std::shared_ptr<const TriangulationData> triangulation;
};

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ss, TreeType type) {
    switch (type) {
        case TreeType::Join:
            ss << "Join Tree";
            break;
        case TreeType::Split:
            ss << "Split Tree";
            break;
        case TreeType::Contour:
            ss << "Contour Tree";
            break;
        case TreeType::JoinAndSplit:
            ss << "Join and Split Tree";
            break;
        default:
            ss << "Invalid";
    }
    return ss;
}

}  // namespace topology

template <>
struct DataTraits<topology::ContourTreeData> {
    static std::string classIdentifier() { return "org.inviwo.topology.contourtreedata"; }
    static std::string dataName() { return "TTK Contour Tree"; }
    static uvec3 colorCode() { return uvec3(94, 152, 224); }
    static Document info(const topology::ContourTreeData& data) {
        using H = utildoc::TableBuilder::Header;
        using P = Document::PathComponent;
        Document doc;
        doc.append("b", dataName(), {{"style", "color:white;"}});
        utildoc::TableBuilder tb(doc.handle(), P::end());
        if (data.triangulation) {
            tb(H("Dimensionality"), data.triangulation->getTriangulation().getDimensionality());
        }
        if (auto tree = data.getTree()) {
            tb(H("Arcs"), tree->getNumberOfSuperArcs());
            tb(H("Nodes"), tree->getNumberOfNodes());
            tb(H("Leaves"), tree->getNumberOfLeaves());
            tb(H("Vertices"), tree->getNumberOfVertices());
        }
        return doc;
    }
};

}  // namespace inviwo
