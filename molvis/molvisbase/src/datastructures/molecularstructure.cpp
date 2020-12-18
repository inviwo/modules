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

#include <inviwo/molvisbase/datastructures/molecularstructure.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/util/assertion.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/zip.h>

#include <inviwo/molvisbase/util/molvisutils.h>

#include <fmt/format.h>
#include <limits>

namespace inviwo {

namespace molvis {

namespace detail {

inline double dihedralAngle(const dvec3& p0, const dvec3& p1, const dvec3& p2, const dvec3& p3) {
    const dvec3 b1{p1 - p0};
    const dvec3 b2{p2 - p1};
    const dvec3 b3{p3 - p2};
    const dvec3 c1{glm::cross(b1, b2)};
    const dvec3 c2{glm::cross(b2, b3)};
    return glm::atan(glm::dot(glm::cross(c1, c2), glm::normalize(b2)), glm::dot(c1, c2));
}

void computeDihedralAngles(
    std::unordered_map<int, std::vector<MolecularStructure::BackboneSegment>>& chainSegments,
    const std::unordered_map<ResidueID, size_t>& residueIndices, const MolecularData& data) {
    for (auto& [chainId, segments] : chainSegments) {
        if (segments.size() < 2) continue;

        auto begin = segments.begin();
        auto end = segments.end();

        auto pos = [&](auto idx) { return data.atoms.positions[idx]; };

        if (begin->complete()) {
            begin->psi = detail::dihedralAngle(pos(begin->n.value()), pos(begin->ca.value()),
                                               pos(begin->c.value()), pos((begin + 1)->n.value()));
        }
        if ((end - 1)->complete()) {
            (end - 1)->phi =
                detail::dihedralAngle(pos((end - 2)->c.value()), pos((end - 1)->n.value()),
                                      pos((end - 1)->ca.value()), pos((end - 1)->c.value()));
        }

        for (auto&& [prev, current, next] :
             util::zip(util::as_range(begin, end - 2), util::as_range(begin + 1, end - 1),
                       util::as_range(begin + 2, end))) {
            if (!current.complete()) {
                continue;
            }
            const dvec3& ca{pos(current.ca.value())};
            const dvec3& n{pos(current.n.value())};
            const dvec3& c{pos(current.c.value())};

            if (prev.c.has_value()) {
                current.phi = detail::dihedralAngle(pos(prev.c.value()), n, ca, c);
            }
            if (next.n.has_value()) {
                current.psi = detail::dihedralAngle(n, ca, c, pos(next.n.value()));
            }
        }

        // determine peptide types
        for (auto&& [current, next] :
             util::zip(util::as_range(begin, end - 1), util::as_range(begin + 1, end))) {
            current.type = getPeptideType(
                data.residues[residueIndices.at({current.resId, current.chainId})].aminoacid,
                data.residues[residueIndices.at({next.resId, next.chainId})].aminoacid);
        }
        (end - 1)->type = getPeptideType(
            data.residues[residueIndices.at({(end - 1)->resId, (end - 1)->chainId})].aminoacid,
            AminoAcid::Unknown);
    }
}

std::pair<std::unordered_map<int, std::vector<MolecularStructure::BackboneSegment>>,
          std::vector<size_t>>
computeBackboneSegments(const MolecularData& data,
                        std::unordered_map<int, std::vector<size_t>> chainResidues,
                        std::unordered_map<ResidueID, std::vector<size_t>> residueAtoms,
                        std::unordered_map<ResidueID, size_t> residueIndices) {
    std::unordered_map<int, std::vector<MolecularStructure::BackboneSegment>> chainSegments;
    std::vector<size_t> indices(data.atoms.positions.size());

    const size_t minCount = 4;

    for (auto& chain : chainResidues) {
        std::vector<MolecularStructure::BackboneSegment> segments;

        for (auto& resIdx : chain.second) {
            const auto& res = data.residues[resIdx];

            auto it = residueAtoms.find({res.id, res.chainId});
            if (it == residueAtoms.end()) continue;

            const auto& resAtoms = it->second;
            MolecularStructure::BackboneSegment seg;
            seg.resId = res.id;
            seg.chainId = res.chainId;

            for (auto& atomIdx : resAtoms) {
                indices[atomIdx] = segments.size();
            }

            if (resAtoms.size() < minCount) {
                segments.push_back(seg);
                continue;
            }

            // locate atoms
            for (auto& atomIdx : resAtoms) {
                const Element elem = data.atoms.atomicNumbers[atomIdx];
                switch (data.atoms.atomicNumbers[atomIdx]) {
                    case Element::N:
                        if (!seg.n) seg.n = atomIdx;
                        break;
                    case Element::C:
                        if (data.atoms.fullNames[atomIdx] == "CA") {
                            if (!seg.ca) seg.ca = atomIdx;
                        } else {
                            if (!seg.c) seg.c = atomIdx;
                        }
                        break;
                    case Element::O:
                        if (!seg.o) seg.o = atomIdx;
                        break;
                    default:
                        break;
                }
                if (seg.complete()) break;
            }

            segments.push_back(seg);
        }
        chainSegments.emplace(chain.first, std::move(segments));
    }

    detail::computeDihedralAngles(chainSegments, residueIndices, data);

    return {chainSegments, indices};
}

struct InternalState {
    std::unordered_map<int, std::vector<size_t>> chainResidues;
    std::unordered_map<ResidueID, std::vector<size_t>> residueAtoms;
    std::unordered_map<ResidueID, size_t> residueIndices;
    std::vector<size_t> atomResidueIndices;
    std::unordered_map<int, std::vector<MolecularStructure::BackboneSegment>> chainSegments;
    std::vector<size_t> chainSegmentIndices;
};

InternalState createInternalState(const MolecularData& data) {
    InternalState state;

    const size_t atomCount = data.atoms.positions.size();

    auto atomToStr = [&](size_t index) {
        if (!data.atoms.fullNames.empty()) {
            return fmt::format("{} '{}'", index, data.atoms.fullNames[index]);
        } else {
            return fmt::format("{}", index);
        }
    };

    // create index maps from atoms to residues and residues to atoms
    for (size_t i = 0; i < atomCount; ++i) {
        const auto resId = data.atoms.residueIds[i];
        const auto chainId = data.atoms.chainIds[i];
        auto it = util::find_if(data.residues, [id = resId, chainId](const Residue& r) {
            return r.id == id && r.chainId == chainId;
        });
        if (it == data.residues.end()) {
            throw Exception(fmt::format("Invalid residue ID '{}' in atom {}", resId, atomToStr(i)),
                            IVW_CONTEXT_CUSTOM("MolecularStructure::MolecularStructure()"));
        }
        // add atom to residue
        state.residueAtoms[{resId, chainId}].push_back(i);

        const auto residueIndex = std::distance(data.residues.begin(), it);
        state.atomResidueIndices.push_back(residueIndex);
    }

    for (auto&& [i, res] : util::enumerate(data.residues)) {
        state.residueIndices[{res.id, res.chainId}] = i;
    }

    if (!data.chains.empty()) {
        // update chain information
        for (auto&& [residueIndex, res] : util::enumerate(data.residues)) {
            auto chainIt = util::find_if(data.chains,
                                         [id = res.chainId](const Chain& c) { return c.id == id; });
            if (chainIt == data.chains.end()) {
                throw Exception(fmt::format("Invalid chain ID '{}' in residue {} '{}'", res.chainId,
                                            res.id, aminoacid::symbol(res.aminoacid)),
                                IVW_CONTEXT_CUSTOM("MolecularStructure::MolecularStructure()"));
            }
            state.chainResidues[res.chainId].push_back(residueIndex);
        }

        if (!data.atoms.atomicNumbers.empty()) {
            auto result = detail::computeBackboneSegments(data, state.chainResidues,
                                                          state.residueAtoms, state.residueIndices);
            state.chainSegments = std::move(result.first);
            state.chainSegmentIndices = std::move(result.second);
        }
    }

    return state;
}

void verifyData(const MolecularData& data) {
    const size_t atomCount = data.atoms.positions.size();

    // attributes of atoms in MolecularData can be either empty or must match the number of atoms
    auto sizeValid = [atomCount](const auto& v) { return (v.empty() || (v.size() == atomCount)); };

    if (!sizeValid(data.atoms.bFactors)) {
        throw Exception(fmt::format("Inconsistent MolecularData: expected {} b Factors, found {}",
                                    atomCount, data.atoms.bFactors.size()),
                        IVW_CONTEXT_CUSTOM("MolecularStructure::MolecularStructure()"));
    }
    if (!sizeValid(data.atoms.modelIds)) {
        throw Exception(fmt::format("Inconsistent MolecularData: expected {} model IDs, found {}",
                                    atomCount, data.atoms.modelIds.size()),
                        IVW_CONTEXT_CUSTOM("MolecularStructure::MolecularStructure()"));
    }
    if (!sizeValid(data.atoms.chainIds)) {
        throw Exception(fmt::format("Inconsistent MolecularData: expected {} chain IDs, found {}",
                                    atomCount, data.atoms.chainIds.size()),
                        IVW_CONTEXT_CUSTOM("MolecularStructure::MolecularStructure()"));
    }
    if (!sizeValid(data.atoms.residueIds)) {
        throw Exception(fmt::format("Inconsistent MolecularData: expected {} residue Ids, found {}",
                                    atomCount, data.atoms.residueIds.size()),
                        IVW_CONTEXT_CUSTOM("MolecularStructure::MolecularStructure()"));
    }
    if (!sizeValid(data.atoms.atomicNumbers)) {
        throw Exception(
            fmt::format("Inconsistent MolecularData: expected {} atomic numbers, found {}",
                        atomCount, data.atoms.atomicNumbers.size()),
            IVW_CONTEXT_CUSTOM("MolecularStructure::MolecularStructure()"));
    }
    if (!sizeValid(data.atoms.fullNames)) {
        throw Exception(fmt::format("Inconsistent MolecularData: expected {} full names, found {}",
                                    atomCount, data.atoms.fullNames.size()),
                        IVW_CONTEXT_CUSTOM("MolecularStructure::MolecularStructure()"));
    }
}

}  // namespace detail

MolecularStructure::MolecularStructure(MolecularData data) : data_{std::move(data)} {
    detail::verifyData(data_);

    if (!data_.atoms.residueIds.empty() && !data_.residues.empty() &&
        !data_.atoms.chainIds.empty()) {
        auto state = detail::createInternalState(data_);
        chainResidues_ = std::move(state.chainResidues);
        residueAtoms_ = std::move(state.residueAtoms);
        residueIndices_ = std::move(state.residueIndices);
        atomResidueIndices_ = std::move(state.atomResidueIndices);
        chainSegments_ = std::move(state.chainSegments);
        chainSegmentIndices_ = std::move(state.chainSegmentIndices);
    }
}

const MolecularData& MolecularStructure::data() const { return data_; }

const Atoms& MolecularStructure::atoms() const { return data_.atoms; }

const std::vector<Residue>& MolecularStructure::residues() const { return data_.residues; }

const std::vector<Chain>& MolecularStructure::chains() const { return data_.chains; }

const std::vector<Bond>& MolecularStructure::bonds() const { return data_.bonds; }

std::optional<size_t> MolecularStructure::getAtomIndex(std::string_view fullAtomName, int residueId,
                                                       int chainId) const {
    if (auto resIt = residueAtoms_.find({residueId, chainId}); resIt != residueAtoms_.end()) {
        auto pred = [&](size_t i) {
            return ((data_.atoms.fullNames[i] == fullAtomName) &&
                    (data_.atoms.residueIds[i] == residueId) &&
                    (data_.atoms.chainIds[i] == chainId));
        };

        if (auto atomIt = util::find_if(resIt->second, pred); atomIt != resIt->second.end()) {
            return std::distance(resIt->second.begin(), atomIt);
        }
    }
    return std::nullopt;
}

bool MolecularStructure::hasAtoms() const { return !data_.atoms.positions.empty(); }

bool MolecularStructure::hasResidues() const { return !atomResidueIndices_.empty(); }

bool MolecularStructure::hasResidue(int residueId, int chainId) const {
    return residueAtoms_.find({residueId, chainId}) != residueAtoms_.end();
}

bool MolecularStructure::hasChains() const { return !chainResidues_.empty(); }

bool MolecularStructure::hasChain(int chainId) const {
    return chainResidues_.find(chainId) != chainResidues_.end();
}

const std::vector<size_t>& MolecularStructure::getResidueAtoms(int residueId, int chainId) const {
    if (auto it = residueAtoms_.find({residueId, chainId}); it != residueAtoms_.end()) {
        return it->second;
    } else {
        throw Exception(fmt::format("Residue with ID '{}' and chain ID '{}' does not exist",
                                    residueId, chainId),
                        IVW_CONTEXT);
    }
}

const std::vector<size_t>& MolecularStructure::getChainResidues(int chainId) const {
    if (auto it = chainResidues_.find(chainId); it != chainResidues_.end()) {
        return it->second;
    } else {
        throw Exception(fmt::format("Chain with chain ID '{}' does not exist", chainId),
                        IVW_CONTEXT);
    }
}

auto MolecularStructure::getBackboneSegments(int chainId) const
    -> const std::vector<BackboneSegment>& {
    if (auto it = chainSegments_.find(chainId); it != chainSegments_.end()) {
        return it->second;
    } else {
        throw Exception(fmt::format("Chain with chain ID '{}' does not exist", chainId),
                        IVW_CONTEXT);
    }
}

const std::vector<size_t>& MolecularStructure::getResidueIndices() const {
    return atomResidueIndices_;
}

const std::vector<size_t>& MolecularStructure::getBackboneSegmentIndices() const {
    return chainSegmentIndices_;
}

}  // namespace molvis

}  // namespace inviwo
