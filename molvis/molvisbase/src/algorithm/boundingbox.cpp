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
#include <inviwo/core/algorithm/boundingbox.h>

#include <inviwo/core/util/zip.h>
#include <numeric>
#include <utility>
#include <ranges>

namespace inviwo {

namespace molvis {

std::optional<dmat4> calcBoundingBox(const MolecularStructure& structure) {
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
        return std::nullopt;
    }

    auto m = glm::scale(worldMax - worldMin);
    m[3] = dvec4(worldMin, 1.0);
    return m;
}

std::function<std::optional<dmat4>()> boundingBox(const MolecularStructureInport& structure) {
    return [port = &structure]() -> std::optional<mat4> {
        if (port->hasData()) {
            return calcBoundingBox(*port->getData());
        } else {
            return std::nullopt;
        }
    };
}

std::function<std::optional<dmat4>()> boundingBox(const MolecularStructureMultiInport& structures) {
    return [&structures]() -> std::optional<mat4> {
        if (structures.hasData()) {
            auto dataView = structures |
                                 std::views::transform([](auto ptr) -> decltype(auto) { return *ptr; });
            return util::calcBoundingBox(dataView);
        } else {
            return std::nullopt;
        }
    };
}

std::function<std::optional<dmat4>()> boundingBox(
    const MolecularStructureFlatMultiInport& structures) {
    return [&structures]() -> std::optional<mat4> {
        if (structures.hasData()) {
            return calcBoundingBox(
                util::as_range(structures) |
                std::views::transform([](auto ptr) -> decltype(auto) { return *ptr; }));
        } else {
            return std::nullopt;
        }
    };
}

std::function<std::optional<dmat4>()> boundingBox(const MolecularStructureOutport& structure) {
    return [&structure]() -> std::optional<mat4> {
        if (structure.hasData()) {
            return calcBoundingBox(*structure.getData());
        } else {
            return std::nullopt;
        }
    };
}

}  // namespace molvis

}  // namespace inviwo
