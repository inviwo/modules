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

#include <inviwo/topologytoolkit/processors/contourtreetodataframe.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo ContourTreeToDataFrame::processorInfo_{
    "org.inviwo.ContourTreeToDataFrame",  // Class identifier
    "Contour Tree To Data Frame",         // Display name
    "Topology",                           // Category
    CodeState::Experimental,              // Code state
    "CPU, Topology, TTK, Contour Tree",   // Tags
};
const ProcessorInfo ContourTreeToDataFrame::getProcessorInfo() const { return processorInfo_; }

ContourTreeToDataFrame::ContourTreeToDataFrame()
    : treeType_("treeType", "Tree Type", "", InvalidationLevel::Valid) {

    addPort(inport_);
    addPort(outport_);

    addProperty(treeType_);

    treeType_.setReadOnly(true);

    inport_.onChange([this]() {
        treeType_.set([this]() -> std::string {
            if (inport_.hasData()) {
                /*
                // FIXME: does not work right now due to bug in TTK (see topology::ContourTreeData)
                const bool jointree = inport_.getData()->tree->isJT();
                const bool splittree = inport_.getData()->tree->isST();
                if (jointree && splittree) {
                    return "Join-Split Tree";
                } else if (jointree) {
                    return "Join Tree";
                } else if (splittree) {
                    return "Split Tree";
                } else {
                    return "Contour Tree";
                }
                */
                return toString(inport_.getData()->type);
            } else {
                return "";
            }
        }());
    });
}

void ContourTreeToDataFrame::process() {
    auto tree = inport_.getData()->getTree();

    const size_t numNodes = tree->getNumberOfNodes();

    // create a mesh with critical points and arcs
    auto triangulation = inport_.getData()->triangulation;
    auto compute = [&](const auto buffer) {
        using ValueType = util::PrecisionValueType<decltype(buffer)>;
        using PrimitiveType = typename DataFormat<ValueType>::primitive;

        std::vector<int> vertexIDs(numNodes);
        std::vector<unsigned char> upFlag(numNodes);
        std::vector<unsigned char> downFlag(numNodes);
        std::vector<int> valenceUp(numNodes);
        std::vector<int> valenceDown(numNodes);
        std::vector<PrimitiveType> scalars(numNodes);

        const auto &scalarValues = buffer->getDataContainer();

        for (ttk::ftm::idNode i = 0; i < numNodes; ++i) {
            auto node = tree->getNode(i);
            const bool up = node->getNumberOfUpSuperArcs() > 0;
            const bool down = node->getNumberOfDownSuperArcs() > 0;

            vertexIDs[i] = node->getVertexId();
            upFlag[i] = up ? 1 : 0;
            downFlag[i] = down ? 1 : 0;
            valenceUp[i] = node->getNumberOfUpSuperArcs();
            valenceDown[i] = node->getNumberOfDownSuperArcs();
            scalars[i] = scalarValues[vertexIDs[i]];
        }

        // convert critical points of ttk::ContourTree into a DataFrame
        //
        // vertex ID (int), up (uchar), down (uchar), valence up (int),
        // valence down (int), scalar (PrimitiveType)
        auto dataframe = std::make_shared<DataFrame>();

        dataframe->addColumnFromBuffer("VertexID", util::makeBuffer<int>(std::move(vertexIDs)));
        dataframe->addColumnFromBuffer("Up Flag",
                                       util::makeBuffer<unsigned char>(std::move(upFlag)));
        dataframe->addColumnFromBuffer("Down Flag",
                                       util::makeBuffer<unsigned char>(std::move(downFlag)));
        dataframe->addColumnFromBuffer("Valence Up", util::makeBuffer<int>(std::move(valenceUp)));
        dataframe->addColumnFromBuffer("Valence Down",
                                       util::makeBuffer<int>(std::move(valenceDown)));
        dataframe->addColumnFromBuffer("Scalar Value",
                                       util::makeBuffer<PrimitiveType>(std::move(scalars)));
        dataframe->updateIndexBuffer();

        return dataframe;
    };

    auto dataframe =
        triangulation->getScalarValues()
            ->getRepresentation<BufferRAM>()
            ->dispatch<std::shared_ptr<DataFrame>, dispatching::filter::Scalars>(compute);

    outport_.setData(dataframe);
}

}  // namespace inviwo
