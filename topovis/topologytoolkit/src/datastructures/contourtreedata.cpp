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

#include <inviwo/topologytoolkit/datastructures/contourtreedata.h>

namespace inviwo {

namespace topology {

ttk::ftm::FTMTree_MT* ContourTreeData::getTree() const {
    switch (type) {
        case TreeType::Join:
            return tree->getTree(ttk::ftm::TreeType::Join);
        case TreeType::Split:
            return tree->getTree(ttk::ftm::TreeType::Split);
        case TreeType::Contour:
        case TreeType::JoinAndSplit:
        default:
            return tree.get();
    }
}


void ContourTreeData::setSegments(const std::vector<int>& segments) {
    setSegments(std::vector<int>(segments));
}

void ContourTreeData::setSegments(std::vector<int>&& segments) {
    const auto numelems =
        triangulation->isUniformGrid() ? glm::compMul(triangulation->getGridDimensions()) : triangulation->getPoints().size();
    if (segments.size() != numelems) {
        throw TTKException("Mismatch in range (" + std::to_string(segments.size()) + " segments, " +
                           std::to_string(numelems) + " vertices)");
    }
    segments_ = std::move(segments);
}

std::vector<int>& ContourTreeData::getSegments() {
    const auto numelems = triangulation->isUniformGrid()
                              ? glm::compMul(triangulation->getGridDimensions())
                              : triangulation->getPoints().size();
    if (segments_.size() != numelems) {
        segments_.resize(numelems);
        std::iota(segments_.begin(), segments_.end(), 0);
    }
    return segments_;
}

const std::vector<int>& ContourTreeData::getSegments() const {
    const auto numelems = triangulation->isUniformGrid()
                              ? glm::compMul(triangulation->getGridDimensions())
                              : triangulation->getPoints().size();
    if (segments_.size() != numelems) {
        segments_.resize(numelems);
        std::iota(segments_.begin(), segments_.end(), 0);
    }
    return segments_;
}

}  // namespace topology

}  // namespace inviwo
