/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2020 Inviwo Foundation
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

#include <inviwo/molvisbase/util/molvisutils.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/exception.h>
#include <inviwo/core/util/zip.h>

#include <inviwo/molvisbase/util/atomicelement.h>

#include <fmt/format.h>

namespace inviwo {

namespace molvis {

namespace {

template <typename T>
T square(T a) {
    return a * a;
}

bool covalentBondHeuristics(Element element1, const dvec3& pos1, Element element2,
                            const dvec3& pos2) {
    const dvec3 delta{pos2 - pos1};
    const double distSq = glm::dot(delta, delta);

    const double covalentDist =
        element::covalentRadius(element1) + element::covalentRadius(element2);
    const double dMinSq = square(covalentDist - 0.5);
    const double dMaxSq = square(covalentDist + 0.3);
    return (dMinSq < distSq) && (distSq < dMaxSq);
}

}  // namespace

std::optional<Residue> findResidue(const MolecularData& data, size_t residueId, size_t chainId) {
    auto it = util::find_if(data.residues,
                            [&](auto& r) { return (r.id == residueId) && (r.chainId == chainId); });
    if (it != data.residues.end()) {
        return *it;
    } else {
        return std::nullopt;
    }
}

std::optional<Chain> findChain(const MolecularData& data, size_t chainId) {
    auto it = util::find_if(data.chains, [id = chainId](auto& r) { return r.id == id; });
    if (it != data.chains.end()) {
        return *it;
    } else {
        return std::nullopt;
    }
}

std::optional<size_t> getGlobalAtomIndex(const Atoms& atoms, const std::string& fullAtomName,
                                         size_t residueId, size_t chainId) {
    if ((atoms.residueIds.size() != atoms.chainIds.size()) ||
        (atoms.residueIds.size() != atoms.fullNames.size())) {
        throw Exception(
            fmt::format("Number of residue IDs ({}) does not match size of chain IDs ({}) or full "
                        "atom names ({})",
                        atoms.residueIds.size(), atoms.chainIds.size(), atoms.fullNames.size()),
            IVW_CONTEXT_CUSTOM("getGlobalAtomIndex"));
    }

    for (size_t i = 0; i < atoms.fullNames.size(); ++i) {
        if ((atoms.fullNames[i] == fullAtomName) && (atoms.residueIds[i] == residueId) &&
            (atoms.chainIds[i] == chainId)) {
            return i;
        }
    }
    return std::nullopt;
}

std::vector<unsigned char> getAtomicNumbers(const Atoms& atoms) {
    return util::transform(atoms.fullNames, [](const std::string& name) {
        return static_cast<unsigned char>(element::fromFullName(name));
    });
}

std::vector<Bond> computeCovalentBonds(const Atoms& atoms) {
    if (atoms.positions.empty()) return {};

    if (atoms.positions.size() != atoms.atomicNumbers.size()) {
        throw Exception(
            fmt::format("Number of atoms ({}) does not match size of atomic numbers ({})",
                        atoms.positions.size(), atoms.atomicNumbers.size()),
            IVW_CONTEXT_CUSTOM("computeCovalentBonds"));
    }

    const double maxCovalentBondLength = 4.0;
    const dvec3 cellSize{maxCovalentBondLength};

    // create uniform grid enclosing all atoms +- 1.0Å
    dvec3 min{atoms.positions.front()};
    dvec3 max{atoms.positions.front()};
    for (size_t i = 0; i < atoms.positions.size(); ++i) {
        min = glm::min(min, atoms.positions[i]);
        max = glm::max(max, atoms.positions[i]);
    }
    min -= 1.0;
    max += 1.0;

    const size3_t dims{glm::max(size3_t(1), size3_t((max - min) / cellSize))};
    const dvec3 cellExt{(max - min) / dvec3{dims}};

    std::vector<int> cells(glm::compMul(dims), -1);
    std::vector<std::vector<size_t>> cellData;

    auto cellCoord = [&](const dvec3& pos) -> size3_t {
        return glm::clamp(size3_t((pos - min) / cellExt), size3_t(0), dims - size_t{1});
    };

    util::IndexMapper3D im(dims);
    for (auto&& [i, pos] : util::enumerate(atoms.positions)) {
        const auto cellIndex = im(cellCoord(pos));
        if (cells[cellIndex] == -1) {
            cells[cellIndex] = static_cast<int>(cellData.size());
            cellData.push_back({});
        }
        cellData[cells[cellIndex]].push_back(i);
    }

    auto covalentBond = [&](size_t atom1, size_t atom2) {
        return covalentBondHeuristics(
            element::element(atoms.atomicNumbers[atom1]), atoms.positions[atom1],
            element::element(atoms.atomicNumbers[atom2]), atoms.positions[atom2]);
    };

    std::vector<Bond> bonds;
    for (auto&& [atom1, pos] : util::enumerate(atoms.positions)) {
        const auto cell = cellCoord(pos);
        const auto minCell = cell - size3_t(1);
        const auto maxCell = glm::min(cell + size3_t(1), dims - size3_t{1});

        for (size_t z = minCell.z; z <= maxCell.z; ++z) {
            for (size_t y = minCell.y; y <= maxCell.y; ++y) {
                for (size_t x = minCell.x; x <= maxCell.x; ++x) {
                    const auto cellIndex = im(size3_t{x, y, z});
                    if (cells[cellIndex] > -1) {
                        for (auto atom2 : cellData[cells[cellIndex]]) {
                            if ((atom1 < atom2) && covalentBond(atom1, atom2)) {
                                bonds.push_back({atom1, atom2});
                            }
                        }
                    }
                }
            }
        }
    }
    return bonds;
}

std::shared_ptr<Mesh> createMesh(const MolecularStructure& s) {
    if (s.get().atoms.positions.empty()) {
        return std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    }

    std::vector<vec3> positions{
        util::transform(s.get().atoms.positions, [](auto& p) { return vec3{p}; })};
    std::vector<vec4> colors;
    std::vector<float> radius;
    // std::vector<uint32_t> picking;
    const size_t atomCount = s.get().atoms.positions.size();

    for (auto a : s.get().atoms.atomicNumbers) {
        const auto elem = static_cast<Element>(a);

        colors.emplace_back(element::color(elem));
        radius.emplace_back(static_cast<float>(element::vdwRadius(elem)));
    }
    if (s.get().atoms.atomicNumbers.empty()) {
        // fall-back if atomic numbers are not available
        colors.resize(atomCount, element::color(Element::Unknown));
        radius.resize(atomCount, static_cast<float>(element::vdwRadius(Element::Unknown)));
    }

    auto mesh = std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    mesh->addBuffer(BufferType::PositionAttrib, util::makeBuffer(std::move(positions)));
    mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(std::move(colors)));
    mesh->addBuffer(BufferType::RadiiAttrib, util::makeBuffer(std::move(radius)));
    // mesh->addBuffer(BufferType::PickingAttrib, util::makeBuffer(std::move(picking)));

    // atoms
    std::vector<uint32_t> indices(atomCount);
    std::iota(indices.begin(), indices.end(), 0);
    mesh->addIndices(Mesh::MeshInfo(DrawType::Points, ConnectivityType::None),
                     util::makeIndexBuffer(std::move(indices)));

    // bonds
    if (!s.get().bonds.empty()) {
        indices.clear();
        indices.reserve(s.get().bonds.size() * 2);
        for (const auto& bond : s.get().bonds) {
            indices.push_back(static_cast<uint32_t>(bond.first));
            indices.push_back(static_cast<uint32_t>(bond.second));
        }
        mesh->addIndices(Mesh::MeshInfo(DrawType::Lines, ConnectivityType::None),
                         util::makeIndexBuffer(std::move(indices)));
    }

    return mesh;
}

}  // namespace molvis

}  // namespace inviwo
