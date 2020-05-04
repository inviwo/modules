/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2020 Inviwo Foundation
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

#include <inviwo/topologytoolkit/processors/contourtree.h>
#include <inviwo/topologytoolkit/utils/ttkutils.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/geometry/meshram.h>
#include <inviwo/core/common/inviwoapplication.h>

#include <inviwo/core/network/networklock.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/topologicalSimplification/TopologicalSimplification.h>
#include <ttk/core/base/ftmTree/FTMTree_MT.h>
#include <warn/pop>

#include <tuple>
#include <algorithm>
#include <map>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo ContourTree::processorInfo_{
    "org.inviwo.ContourTree",            // Class identifier
    "Contour Tree",                      // Display name
    "Topology",                          // Category
    CodeState::Experimental,             // Code state
    "CPU, Topology, TTK, Contour Tree",  // Tags
};
const ProcessorInfo ContourTree::getProcessorInfo() const { return processorInfo_; }

ContourTree::ContourTree()
    : inport_("triangulation")
    , outport_("outport")
    , treeType_("treeType", "Tree Type",
                {
                    {"join", "Join Tree", topology::TreeType::Join},
                    {"split", "Split Tree", topology::TreeType::Split},
                    {"contour", "Contour Tree", topology::TreeType::Contour}
                    //{"joinAndSplit", "Join and Split", topology::TreeType::JoinAndSplit}},
                    // FIXME: for some reason TTK does not support this properly.
                    // The resulting tree has no data
                },
                2)
    , threadCount_("threadCount", "Number of Threads", 1, 1, 100)
    , segmentation_("segmentation", "Segmentation", true)
    , normalization_("normalization", "Normalization", false) {

    addPort(inport_);
    addPort(outport_);

    addProperty(treeType_);
    addProperty(threadCount_);
    addProperty(segmentation_);
    addProperty(normalization_);
}

void ContourTree::process() {
    // Save input and properties needed to calculate ttk contour tree to local variables
    const auto inportData = inport_.getData();
    const auto threadCount = threadCount_.get();
    const auto treeType = treeType_.get();
    const auto segmentation = segmentation_.get();
    const auto normalization = normalization_.get();

    // construction of ttk contour tree
    auto computeTree = [inportData, threadCount, treeType, segmentation,
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

        tree->build<PrimitiveType, ttk::SimplexId>();
        return tree;
    };

    // if the inport changes sometime during the calculation of the contour tree, make sure we
    // save that event so that a new contour tree can be calculated after the computation is
    // done.
    if (inport_.isChanged()) inportChanged_ = true;
    dirty_ |= (treeType_.isModified() || segmentation_.isModified() || normalization_.isModified());

    if ((inportChanged_ || dirty_) && treeIsFinished_) {
        treeIsFinished_ = false;
        inportChanged_ = false;
        dirty_ = false;

        dispatchPool([this, inportData, treeType, computeTree]() {
            auto treeData = std::make_shared<topology::ContourTreeData>();
            treeData->type = treeType;
            treeData->triangulation = inportData;

            treeData->tree = inportData->getScalarValues()
                                 ->getRepresentation<BufferRAM>()
                                 ->dispatch<std::shared_ptr<topology::ContourTree>,
                                            dispatching::filter::Scalars>(computeTree);

            dispatchFront([this, treeData]() {
                treeData_ = treeData;
                invalidate(InvalidationLevel::InvalidOutput);
            });

            treeIsFinished_ = true;
        });
    }

    outport_.setData(treeData_);
}

}  // namespace inviwo
