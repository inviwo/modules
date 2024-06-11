/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2024 Inviwo Foundation
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

#include <inviwo/graphviz/processors/layoutmergetree.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/core/util/zip.h>
#include <inviwo/core/datastructures/geometry/typedmesh.h>

#include <cgraph.h>
#include <gvc.h>
#include <gvplugin.h>
#include <gvplugin_layout.h>

extern gvplugin_library_t gvplugin_dot_layout_LTX_library;

/*
#include "builtins.h"

extern gvplugin_library_t gvplugin_dot_layout_LTX_library;
extern gvplugin_library_t gvplugin_neato_layout_LTX_library;
extern gvplugin_library_t gvplugin_core_LTX_library;
extern gvplugin_library_t gvplugin_quartz_LTX_library;
extern gvplugin_library_t gvplugin_visio_LTX_library;

void loadGraphvizLibraries(GVC_t *gvc) {
    gvAddLibrary(gvc, &gvplugin_dot_layout_LTX_library);
    gvAddLibrary(gvc, &gvplugin_neato_layout_LTX_library);
    gvAddLibrary(gvc, &gvplugin_core_LTX_library);
    gvAddLibrary(gvc, &gvplugin_quartz_LTX_library);
    gvAddLibrary(gvc, &gvplugin_visio_LTX_library);
}

#include "/Users/wizardofkneup/graphviz/lib/gvc/gvplugin.h" //
#include "gvc.h"

extern lt_symlist_t lt_preloaded_symbols[];
void loadGraphvizLibraries(GVC_t *gvc);

*/

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo LayoutMergeTree::processorInfo_{
    "org.inviwo.LayoutMergeTree",  // Class identifier
    "Layout Merge Tree",           // Display name
    "Undefined",                   // Category
    CodeState::Experimental,       // Code state
    Tags::None,                    // Tags
    R"(<Explanation of how to use the processor.>)"_unindentHelp};

const ProcessorInfo LayoutMergeTree::getProcessorInfo() const { return processorInfo_; }

LayoutMergeTree::LayoutMergeTree()
    : Processor{}
    , dataFrame_{"dataFrame", ""_help}
    , outport_{"outport", "merge tree mesh"_help}
    , upColumn_{"upColumn", "Up Column", dataFrame_}
    , downColumn_{"downColumn", "Down Column", dataFrame_} {

    addPorts(dataFrame_, outport_);
    addProperties(upColumn_, downColumn_);
}

void LayoutMergeTree::process() {

    const auto ups = dataFrame_.getData()->getColumn(upColumn_.getSelectedValue());
    const auto downs = dataFrame_.getData()->getColumn(downColumn_.getSelectedValue());

    auto* upsTyped = dynamic_cast<const TemplateColumn<std::int64_t>*>(ups.get());
    auto* downsTyped = dynamic_cast<const TemplateColumn<std::int64_t>*>(downs.get());

    if (!upsTyped || !downsTyped) {
        throw Exception("Unexpected types");
    }

    StrBuffer buff{"digraph G {{\n"};

    for (auto&& [up, down] : util::zip(*upsTyped, *downsTyped)) {
        buff.append("{} -> {};\n", up, down);
    }
    buff.append("}}");

    LogInfo(buff.view());

    Agraph_t* G = agmemread(buff.c_str());
    GVC_t* gvc = gvContext();
    gvAddLibrary(gvc, &gvplugin_dot_layout_LTX_library);
    gvLayout(gvc, G, "dot");

    buff.clear();

    size_t startPickId = 0;

    auto mesh =
        std::make_shared<TypedMesh<buffertraits::PositionsBuffer, buffertraits::ColorsBuffer,
                                   buffertraits::PickingBuffer, buffertraits::IndexBuffer>>();
    auto& ib = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None)->getDataContainer();
    auto& lines = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None)->getDataContainer();
    mesh->reserveSizeInVertexBuffer(downsTyped->getSize());

    std::unordered_map<size_t, uint32_t> nodeToVertex;

        for (auto&& [index, node] : util::enumerate(*downsTyped)) {
        Agnode_t* n = agnode(G, const_cast<char*>(buff.replace("{}", node).c_str()), 0);
        if (n != nullptr) {
            auto& coord = ND_coord(n);

            const auto pos = vec3{static_cast<float>(coord.x), static_cast<float>(coord.y), 0.0};
            const auto color = vec4{1.0, 0.7, 0.3, 1.0};
            const auto pi = startPickId + index;
            mesh->addVertex(pos, color, pi, index);
            ib.push_back(static_cast<uint32_t>(index));
            nodeToVertex[node] = static_cast<uint32_t>(index);
        }
    }

    gvFreeLayout(gvc, G);
    agclose(G);
    gvFreeContext(gvc);

    for (auto&& [up, down] : util::zip(*upsTyped, *downsTyped)) {
        auto upIt = nodeToVertex.find(up);
        auto downIt = nodeToVertex.find(down);
        if (upIt != nodeToVertex.end() && downIt != nodeToVertex.end()) {
            lines.push_back(upIt->second);
            lines.push_back(downIt->second);
        }
    }

    outport_.setData(mesh);
}

}  // namespace inviwo
