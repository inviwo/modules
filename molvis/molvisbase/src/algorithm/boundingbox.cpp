/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2026 Inviwo Foundation
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

#include <inviwo/molvisbase/algorithm/boundingbox.h>

#include <inviwo/molvisbase/datastructures/molecularstructure.h>

#include <inviwo/core/util/zip.h>
#include <numeric>
#include <utility>

namespace inviwo {

namespace molvis {

mat4 boundingBox(const MolecularStructure& structure) {
    dvec3 worldMin(std::numeric_limits<double>::max());
    dvec3 worldMax(std::numeric_limits<double>::lowest());

    if (structure.hasAtoms()) {
        if (!structure.atoms().atomicNumbers.empty()) {
            for (auto&& [pos, element] :
                 util::zip(structure.atoms().positions, structure.atoms().atomicNumbers)) {
                const dvec3 radius{element::vdwRadius(element)};
                worldMin = glm::min(worldMin, pos - radius);
                worldMax = glm::max(worldMax, pos + radius);
            }
        } else {
            for (const auto& pos : structure.atoms().positions) {
                const dvec3 radius{element::vdwRadius(Element::Unknown)};
                worldMin = glm::min(worldMin, pos - radius);
                worldMax = glm::max(worldMax, pos + radius);
            }
        }
    } else {
        worldMin = worldMax = dvec3(0.0, 0.0, 0.0);
    }

    auto m = glm::scale(vec3(worldMax) - vec3(worldMin));
    m[3] = vec4(worldMin, 1.0f);
    return m;
}

template <typename const_iterator>
mat4 boundingBox(const_iterator begin, const_iterator end) {
    if (begin == end) return mat4(0.0f);

    vec3 worldMin(std::numeric_limits<float>::max());
    vec3 worldMax(std::numeric_limits<float>::lowest());

    const std::array<vec3, 8> corners = {vec3{0, 0, 0}, vec3{1, 0, 0}, vec3{1, 1, 0},
                                         vec3{0, 1, 0}, vec3{0, 0, 1}, vec3{1, 0, 1},
                                         vec3{1, 1, 1}, vec3{0, 1, 1}};
    bool validBbox = false;
    while (begin != end) {
        auto structure = *begin++;
        if (!structure->hasAtoms()) continue;
        validBbox = true;
        auto bb = boundingBox(*structure);
        for (const auto& corner : corners) {
            const auto point = vec3(bb * vec4(corner, 1.f));
            worldMin = glm::min(worldMin, point);
            worldMax = glm::max(worldMax, point);
        }
    }
    if (!validBbox) {
        worldMin = worldMax = vec3(0.0f, 0.0f, 0.0f);
    }

    auto m = glm::scale(worldMax - worldMin);
    m[3] = vec4(worldMin, 1.0f);
    return m;
}

std::function<std::optional<mat4>()> boundingBox(const MolecularStructureInport& structure) {
    return [port = &structure]() -> std::optional<mat4> {
        if (port->hasData()) {
            return boundingBox(*port->getData());
        } else {
            return std::nullopt;
        }
    };
}

std::function<std::optional<mat4>()> boundingBox(const MolecularStructureMultiInport& structures) {
    return [port = &structures]() -> std::optional<mat4> {
        if (port->hasData()) {
            return boundingBox(port->begin(), port->end());
        } else {
            return std::nullopt;
        }
    };
}

std::function<std::optional<mat4>()> boundingBox(
    const MolecularStructureFlatMultiInport& structures) {
    return [port = &structures]() -> std::optional<mat4> {
        if (port->hasData()) {
            return boundingBox(port->begin(), port->end());
        } else {
            return std::nullopt;
        }
    };
}

std::function<std::optional<mat4>()> boundingBox(const MolecularStructureOutport& structure) {
    return [port = &structure]() -> std::optional<mat4> {
        if (port->hasData()) {
            return boundingBox(*port->getData());
        } else {
            return std::nullopt;
        }
    };
}

}  // namespace molvis

}  // namespace inviwo
