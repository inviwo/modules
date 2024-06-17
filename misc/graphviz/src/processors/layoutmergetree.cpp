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
#include <inviwo/core/util/raiiutils.h>
#include <inviwo/core/datastructures/geometry/typedmesh.h>
#include <inviwo/core/interaction/events/pickingevent.h>

#include <inviwo/graphviz/graphvizutil.h>

#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>

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
    , edges_{"edges",
             "A data frame with a row per edge in the tree. "
             "The up and down Column properties defined the edge nodes"_help}
    , nodes_{"nodes",
             "A data frame with a row per node in the tree, "
             "the Node Id Column property should contain the "
             "same node ids as in the edge table"_help}
    , bnl_{"bnl", ""_help, {}}
    , outport_{"outport", "merge tree mesh"_help}
    , upColumn_{"upColumn", "Up Column", edges_}
    , downColumn_{"downColumn", "Down Column", edges_}
    , nodeIdColumn_{"nodeIdColumn", "Node Id Column", nodes_}
    , nodeColorColumn_{"nodeColorColumn", "Node Color Column", nodes_}
    , nodeColorMap_{"nodeColorMap", "Node Color Map"}
    , pm_{this, 1, [this](PickingEvent* event) { picking(event); }} {

    addPorts(edges_, nodes_, bnl_, outport_);
    addProperties(upColumn_, downColumn_, nodeIdColumn_, nodeColorColumn_, nodeColorMap_);
}

LayoutMergeTree::~LayoutMergeTree() {
    if (gvc) {
        gvFreeContext(gvc);
    }
}

void LayoutMergeTree::process() {
    if (!gvc) {
        gvc = gvContext();
        util::loadGraphvizLibraries(gvc);
    }

    const auto ups = edges_.getData()->getColumn(upColumn_.getSelectedValue());
    const auto downs = edges_.getData()->getColumn(downColumn_.getSelectedValue());
    const auto ids = nodes_.getData()->getColumn(nodeIdColumn_.getSelectedValue());

    const auto colors = nodes_.getData()->getColumn(nodeColorColumn_.getSelectedValue());

    auto* upsTyped = dynamic_cast<const TemplateColumn<std::int64_t>*>(ups.get());
    auto* downsTyped = dynamic_cast<const TemplateColumn<std::int64_t>*>(downs.get());
    auto* idsTyped = dynamic_cast<const TemplateColumn<std::int64_t>*>(ids.get());

    if (!upsTyped || !downsTyped || !idsTyped) {
        throw Exception("Unexpected types");
    }

    StrBuffer buff{"digraph G {{\n"};
    for (auto&& [up, down] : util::zip(*upsTyped, *downsTyped)) {
        buff.append("{} -> {};\n", up, down);
    }
    buff.append("}}");

    Agraph_t* G = agmemread(buff.c_str());
    gvLayout(gvc, G, "dot");
    util::OnScopeExit freeG{[&]() {
        gvFreeLayout(gvc, G);
        agclose(G);
    }};

    buff.clear();

    std::unordered_map<size_t, size_t> nodeToIndex;
    for (auto&& [index, node] : util::enumerate(*idsTyped)) {
        nodeToIndex[node] = index;
    }

    auto mesh =
        std::make_shared<TypedMesh<buffertraits::PositionsBuffer, buffertraits::ColorsBuffer,
                                   buffertraits::PickingBuffer, buffertraits::IndexBuffer>>();
    auto& ib = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None)->getDataContainer();
    auto& lines = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None)->getDataContainer();
    mesh->reserveSizeInVertexBuffer(downsTyped->getSize());

    pm_.resize(downsTyped->getSize());
    size_t startPickId = pm_.getPickingId(0);

    const auto colorRange = colors->getRange();

    std::unordered_map<size_t, uint32_t> nodeToVertex;
    for (auto&& [index, node] : util::enumerate(*downsTyped)) {
        Agnode_t* n = agnode(G, const_cast<char*>(buff.replace("{}", node).c_str()), 0);
        if (n != nullptr) {
            auto& coord = ND_coord(n);

            const auto pos = vec3{static_cast<float>(coord.x), static_cast<float>(coord.y), 0.0};
            const auto nodeIndex = nodeToIndex[node];
            const auto color =
                colorRange.x + colors->getAsDouble(nodeIndex) / (colorRange.y - colorRange.x);
            const auto pi = startPickId + index;
            mesh->addVertex(pos, nodeColorMap_->sample(color), pi, index);
            ib.push_back(static_cast<uint32_t>(index));
            nodeToVertex[node] = static_cast<uint32_t>(index);
        }
    }

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

void LayoutMergeTree::picking(const PickingEvent* event) {
    if (event->getPressState() == PickingPressState::None) {
        if (event->getHoverState() == PickingHoverState::Enter) {
            const auto i = event->getPickedId();
            bnl_.highlight(BitSet{i});

            if (auto data = nodes_.getData()) {
                StrBuffer buff{};
                for (auto col : *data) {
                    if (i < col->getSize()) {
                        buff.append(" {}: {}", col->getHeader(), col->getAsString(i));
                    }
                }
                event->setToolTip(buff.view());
            } else {
                event->setToolTip(fmt::format("Row {}", i + 1));
            }

        } else if (event->getHoverState() == PickingHoverState::Exit) {
            bnl_.highlight(BitSet{});
            event->setToolTip("");
        }
    }
    if (event->getPressState() == PickingPressState::Release &&
        event->getPressItem() == PickingPressItem::Primary &&
        std::abs(event->getDeltaPressedPosition().x) < 0.01 &&
        std::abs(event->getDeltaPressedPosition().y) < 0.01) {

        auto selection = bnl_.getSelectedIndices();
        selection.flip(static_cast<uint32_t>(event->getPickedId()));
        bnl_.select(selection);
    }
}

}  // namespace inviwo
