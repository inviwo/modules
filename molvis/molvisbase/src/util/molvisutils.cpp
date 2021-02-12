
/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2021 Inviwo Foundation
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
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/core/util/exception.h>
#include <inviwo/core/util/zip.h>

#include <inviwo/molvisbase/util/atomicelement.h>
#include <inviwo/molvisbase/util/aminoacid.h>

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

template <typename T, typename Pred>
auto find_if_opt(T& cont, Pred pred) -> std::optional<typename T::value_type> {
    using std::begin;
    using std::end;

    auto it = std::find_if(begin(cont), end(cont), pred);
    if (it != end(cont)) {
        return *it;
    } else {
        return std::nullopt;
    }
}

}  // namespace

std::optional<Residue> findResidue(const MolecularData& data, int residueId, int chainId) {
    return find_if_opt(data.residues,
                       [&](auto& r) { return (r.id == residueId) && (r.chainId == chainId); });
}

std::optional<Chain> findChain(const MolecularData& data, int chainId) {
    return find_if_opt(data.chains, [id = chainId](auto& r) { return r.id == id; });
}

std::optional<size_t> getGlobalAtomIndex(const Atoms& atoms, std::string_view fullAtomName,
                                         int residueId, int chainId) {
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

PeptideType getPeptideType(AminoAcid residue, AminoAcid nextResidue) {
    if (residue == AminoAcid::Gly) {
        return PeptideType::Glycine;
    } else if (residue == AminoAcid::Pro) {
        return PeptideType::Proline;
    } else if (nextResidue == AminoAcid::Pro) {
        return PeptideType::PrePro;
    } else {
        return PeptideType::General;
    }
}

PeptideType getPeptideType(std::string_view resName, std::string_view nextResName) {
    if (iCaseCmp(resName, "GLY")) {
        return PeptideType::Glycine;
    } else if (iCaseCmp(resName, "PRO")) {
        return PeptideType::Proline;
    } else if (iCaseCmp(nextResName, "PRO")) {
        return PeptideType::PrePro;
    } else {
        return PeptideType::General;
    }
}

std::vector<Element> getAtomicNumbers(const std::vector<std::string>& fullNames) {
    return util::transform(fullNames,
                           [](const std::string& name) { return element::fromFullName(name); });
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
        return covalentBondHeuristics(atoms.atomicNumbers[atom1], atoms.positions[atom1],
                                      atoms.atomicNumbers[atom2], atoms.positions[atom2]);
    };

    std::vector<Bond> bonds;
    for (auto&& [atom1, pos] : util::enumerate(atoms.positions)) {
        const auto minCell = cellCoord(pos - maxCovalentBondLength);
        const auto maxCell = cellCoord(pos + maxCovalentBondLength);

        for (size_t z = minCell.z; z <= maxCell.z; ++z) {
            for (size_t y = minCell.y; y <= maxCell.y; ++y) {
                for (size_t x = minCell.x; x <= maxCell.x; ++x) {
                    const auto cellIndex = im(size3_t{x, y, z});
                    if (cells[cellIndex] > -1) {
                        for (auto atom2 : cellData[cells[cellIndex]]) {
                            if (glm::distance2(atoms.positions[atom1], atoms.positions[atom2]) <
                                square(maxCovalentBondLength)) {
                                if ((atom1 < atom2) && covalentBond(atom1, atom2)) {
                                    bonds.push_back({atom1, atom2});
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return bonds;
}

std::shared_ptr<Mesh> createMesh(const MolecularStructure& s, bool enablePicking,
                                 uint32_t startId) {
    if (s.atoms().positions.empty()) {
        return std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    }

    std::vector<vec3> positions{
        util::transform(s.atoms().positions, [](const dvec3& p) { return glm::vec3{p}; })};
    std::vector<vec4> colors;
    std::vector<float> radius;
    const size_t atomCount = s.atoms().positions.size();

    for (auto elem : s.atoms().atomicNumbers) {
        colors.emplace_back(element::color(elem));
        radius.emplace_back(static_cast<float>(element::vdwRadius(elem)));
    }
    if (s.atoms().atomicNumbers.empty()) {
        // fall-back if atomic numbers are not available
        colors.resize(atomCount, element::color(Element::Unknown));
        radius.resize(atomCount, static_cast<float>(element::vdwRadius(Element::Unknown)));
    }

    auto mesh = std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    mesh->addBuffer(BufferType::PositionAttrib, util::makeBuffer(std::move(positions)));
    mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(std::move(colors)));
    mesh->addBuffer(BufferType::RadiiAttrib, util::makeBuffer(std::move(radius)));

    if (enablePicking) {
        std::vector<uint32_t> picking(atomCount);
        std::iota(std::begin(picking), std::end(picking), startId);
        mesh->addBuffer(BufferType::PickingAttrib, util::makeBuffer(std::move(picking)));
    }

    // atoms
    std::vector<uint32_t> indices(atomCount);
    std::iota(indices.begin(), indices.end(), 0);
    mesh->addIndices(Mesh::MeshInfo(DrawType::Points, ConnectivityType::None),
                     util::makeIndexBuffer(std::move(indices)));

    // bonds
    if (!s.bonds().empty()) {
        indices.clear();
        indices.reserve(s.bonds().size() * 2);
        for (const auto& bond : s.bonds()) {
            indices.push_back(static_cast<uint32_t>(bond.first));
            indices.push_back(static_cast<uint32_t>(bond.second));
        }
        mesh->addIndices(Mesh::MeshInfo(DrawType::Lines, ConnectivityType::None),
                         util::makeIndexBuffer(std::move(indices)));
    }

    return mesh;
}

Document createToolTip(const MolecularStructure& s, int atomIndex) {
    using H = utildoc::TableBuilder::Header;
    using P = Document::PathComponent;
    const auto& atoms = s.atoms();
    Document doc;
    doc.append("b", fmt::format("Atom {}", atomIndex), {{"style", "color:white;"}});
    utildoc::TableBuilder tb(doc.handle(), P::end());
    const auto& pos = atoms.positions[atomIndex];
    if (!atoms.atomicNumbers.empty()) {
        if (atoms.fullNames.empty()) {
            tb(H("Element"), element::symbol(atoms.atomicNumbers[atomIndex]));
        } else {
            tb(H("Element"),
               fmt::format("{} ('{}')", element::symbol(atoms.atomicNumbers[atomIndex]),
                           atoms.fullNames[atomIndex]));
        }
    } else if (!atoms.fullNames.empty()) {
        tb(H("Full Name"), atoms.fullNames[atomIndex]);
    }
    tb(H("Position"), fmt::format("{:.3f}, {:.3f}, {:.3f}", pos.x, pos.y, pos.z));
    if (!atoms.residueIds.empty()) {
        const auto resId = atoms.residueIds[atomIndex];
        if (!atoms.chainIds.empty()) {
            if (auto res = findResidue(s.data(), resId, atoms.chainIds[atomIndex])) {
                tb(H("Residue"), fmt::format("{} ('{}', id: {})", aminoacid::symbol(res->aminoacid),
                                             res->fullName, res->id));
            }
        } else {
            tb(H("Residue"), atoms.residueIds[atomIndex]);
        }
    }
    if (!atoms.chainIds.empty()) {
        const auto chainId = atoms.chainIds[atomIndex];
        if (auto chain = findChain(s.data(), chainId)) {
            tb(H("Chain"), fmt::format("{} (id: {})", chain->name, chain->id));
        } else {
            tb(H("Chain"), chainId);
        }
    }
    if (!atoms.modelIds.empty()) {
        tb(H("Model"), atoms.modelIds[atomIndex]);
    }
    if (!atoms.bFactors.empty()) {
        tb(H("B Factor"), atoms.bFactors[atomIndex]);
    }
    return doc;
}

}  // namespace molvis

}  // namespace inviwo
