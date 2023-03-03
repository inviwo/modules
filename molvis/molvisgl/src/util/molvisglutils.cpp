/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2022-2023 Inviwo Foundation
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
#include <inviwo/molvisgl/util/molvisglutils.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/buffer/buffer.h>

#include <modules/brushingandlinking/brushingandlinkingmanager.h>
#include <inviwo/molvisgl/util/vertexflags.h>

namespace inviwo {

namespace molvis {

void updateBrushing(const BrushingAndLinkingManager& manager, Mesh& mesh, size_t offset,
                    bool showSelected, bool showHighlighted) {
    const size_t atomCount = mesh.getIndices(0)->getSize();

    auto validIndex = [min = offset, max = offset + atomCount](size_t index) {
        return (index >= min) && (index < max);
    };

    std::vector<unsigned char> mask(atomCount, 0);
    if (showSelected) {
        for (auto idx : manager.getSelectedIndices()) {
            if (validIndex(idx)) {
                mask[idx] |= util::underlyingValue(VertexFlag::Selected);
            }
        }
    }
    if (showHighlighted) {
        for (auto idx : manager.getHighlightedIndices()) {
            if (validIndex(idx)) {
                mask[idx] |= util::underlyingValue(VertexFlag::Highlighted);
            }
        }
    }

    for (auto idx : manager.getFilteredIndices()) {
        if (validIndex(idx)) {
            mask[idx] |= util::underlyingValue(VertexFlag::Filtered);
        }
    }
    mesh.replaceBuffer(mesh.findBuffer(BufferType::TexCoordAttrib).first,
                       Mesh::BufferInfo(BufferType::TexCoordAttrib),
                       util::makeBuffer(std::move(mask)));
}

}  // namespace molvis

}  // namespace inviwo
