/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018 Inviwo Foundation
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

#ifndef IVW_MESHITERATORUTILS_H
#define IVW_MESHITERATORUTILS_H

#include <modules/openmesh/openmeshmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>
#include <inviwo/core/datastructures/buffer/bufferramprecision.h>
#include <inviwo/core/datastructures/geometry/mesh.h>

namespace inviwo {

namespace meshutil {

template <typename Callback>
void forEachTriangle(const Mesh::MeshInfo &info, const IndexBuffer &ib, Callback callback) {
    auto &ram = ib.getRAMRepresentation()->getDataContainer();
    if (info.dt != DrawType::Triangles) {
        std::ostringstream errMsg;
        errMsg << "Only works for triangles, got " << info.dt;
        throw inviwo::Exception(errMsg.str(), IvwContextCustom("meshutil::forEachTriangle"));
    }

    if (ram.size() < 3) {
        throw inviwo::Exception("Not enough indices to create a single triangle",
                                IvwContextCustom("meshutil::forEachTriangle"));
    }

    if (info.ct == ConnectivityType::None) {
        for (size_t i = 0; i < ram.size(); i += 3) {
            callback(ram[i], ram[i + 1], ram[i + 2]);
        }
    }

    else if (info.ct == ConnectivityType::Strip) {
        for (size_t i = 0; i < ram.size() - 2; i++) {
            callback(ram[i], ram[i + 1], ram[i + 2]);
        }
    }

    else if (info.ct == ConnectivityType::Fan) {
        uint32_t a = static_cast<uint32_t>(ram.front());
        for (size_t i = 1; i < ram.size(); i++) {
            callback(a, ram[i], ram[i + 1]);
        }
    }

    else {
        std::ostringstream errMsg;
        errMsg << "ConnectivityType " << info.ct << " not supported";
        throw inviwo::Exception(errMsg.str(), IvwContextCustom("meshutil::forEachTriangle"));
    }
}

template <typename Callback>
void forEachLineSegment(const Mesh::MeshInfo &info, const IndexBuffer &ib, Callback callback) {
    auto &ram = ib.getRAMRepresentation()->getDataContainer();
    if (info.dt != DrawType::Lines) {
        std::ostringstream errMsg;
        errMsg << "Only works for lines, got " << info.dt;
        throw inviwo::Exception(errMsg.str(), IvwContextCustom("meshutil::forEachLineSegment"));
    }

    if (ram.size() < 2) {
        throw inviwo::Exception("Not enough indices to create a single line segment",
                                IvwContextCustom("meshutil::forEachLineSegment"));
    }

    if (info.ct == ConnectivityType::None) {
        for (size_t i = 0; i < ram.size(); i += 2) {
            callback(ram[i], ram[i + 1]);
        }
    }

    else if (info.ct == ConnectivityType::Strip) {
        for (size_t i = 0; i < ram.size() - 1; i++) {
            callback(ram[i], ram[i + 1]);
        }
    }

    else if (info.ct == ConnectivityType::Loop) {
        for (size_t i = 0; i < ram.size() - 1; i++) {
            callback(ram[i], ram[i + 1]);
        }
        callback(ram.back(), ram.front());
    }

    else {
        std::ostringstream errMsg;
        errMsg << "ConnectivityType " << info.ct << " not supported";
        throw inviwo::Exception(errMsg.str(), IvwContextCustom("meshutil::forEachLineSegment"));
    }
}

}  // namespace meshutil
}  // namespace inviwo

#endif  // IVW_MESHITERATORUTILS_H
