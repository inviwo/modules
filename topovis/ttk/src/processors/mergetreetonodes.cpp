/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2023 Inviwo Foundation
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

#include <inviwo/ttk/processors/mergetreetonodes.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MergeTreeToNodes::processorInfo_{
    "org.inviwo.MergeTreeToNodes",  // Class identifier
    "Merge Tree To Nodes",          // Display name
    "VTK",                          // Category
    CodeState::Experimental,        // Code state
    Tag::CPU | Tag{"VTK"},          // Tags
    R"(Create merge tree nodes (with node id, scalar value, critical type and parent id) from VTK critical points and VTK edges)"_unindentHelp};

const ProcessorInfo MergeTreeToNodes::getProcessorInfo() const { return processorInfo_; }

MergeTreeToNodes::MergeTreeToNodes()
    : Processor{}
    , treeCrits_{"treeCrits", "Critical point data (points)"_help}
    , treeEdges_{"treeEdges", "Edges data (cells)"_help}
    , nodeList_{"nodeList", "List of nodes with parent id"_help} {

    addPorts(treeCrits_, treeEdges_, nodeList_);
}

void MergeTreeToNodes::process() {
    // OBS: treeCrits and treeEdges needs to be in the same order as they are parsed from the file!!
    auto treeCrits = treeCrits_.getData();
    auto treeEdges = treeEdges_.getData();
    auto output = std::make_shared<DataFrame>();

    output->addColumn(std::shared_ptr<Column>(treeCrits->getColumn("NodeId")->clone()));
    output->addColumn(std::shared_ptr<Column>(treeCrits->getColumn("Scalar")->clone()));
    output->addColumn(std::shared_ptr<Column>(treeCrits->getColumn("CriticalType")->clone()));

    auto bufferram_downNodes = static_cast<const BufferRAMPrecision<int>*>(
        treeEdges->getColumn("downNodeId")->getBuffer()->getRepresentation<BufferRAM>());
    auto downNodeIds = bufferram_downNodes->getDataContainer();
    auto bufferram_upNodes = static_cast<const BufferRAMPrecision<int>*>(
        treeEdges->getColumn("upNodeId")->getBuffer()->getRepresentation<BufferRAM>());
    auto upNodeIds = bufferram_upNodes->getDataContainer();

    // Find parent id for each node (the root will have no parent)
    std::vector<int> parentIds = {};
    const size_t numRows = treeCrits->getNumberOfRows();
    for (size_t i = 0; i < numRows; i++) {
        auto nodeId = (int)output->getColumn("NodeId")->getAsDouble(i);

        auto it_downNodes = std::find(downNodeIds.begin(), downNodeIds.end(), nodeId);
        auto index = std::distance(downNodeIds.begin(), it_downNodes);

        // try this...
        auto parentId = NULL;
        try {
            parentId = upNodeIds.at(index);
        } catch (const std::exception&) {
            /*throw Exception("Node has no parent: " + nodeId,
                            IVW_CONTEXT_CUSTOM("MergeTreeToNodes"));*/
        }
        parentIds.push_back(parentId);
    }
    output->addColumn("ParentId", parentIds);

    output->updateIndexBuffer();
    nodeList_.setData(output);
}

}  // namespace inviwo
