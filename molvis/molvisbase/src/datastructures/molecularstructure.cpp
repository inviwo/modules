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

#include <inviwo/molvisbase/datastructures/molecularstructure.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/util/assertion.h>
#include <inviwo/core/util/zip.h>

#include <inviwo/molvisbase/util/molvisutils.h>

#include <fmt/format.h>

namespace inviwo {

namespace molvis {

MolecularStructure::MolecularStructure(MolecularData data) : data_{std::move(data)} {
    const size_t atomCount = data_.atoms.positions.size();

    if (!data_.atoms.residueIds.empty() && !data_.residues.empty()) {
        auto atomToStr = [&](size_t index) {
            std::string str = std::to_string(index);
            if (!data_.atoms.fullNames.empty()) {
                str = fmt::format("{} '{}'", str, data_.atoms.fullNames[index]);
            }
            return str;
        };

        // create index maps from atoms to residues and residues to atoms
        for (size_t i = 0; i < atomCount; ++i) {
            const auto resId = data_.atoms.residueIds[i];
            const auto chainId = data_.atoms.chainIds[i];
            auto it = util::find_if(data_.residues,
                                    [id = resId](const Residue& r) { return r.id == id; });
            if (it == data_.residues.end()) {
                throw Exception(
                    fmt::format("Invalid residue ID '{}' in atom {}", resId, atomToStr(i)),
                    IVW_CONTEXT);
            }
            if (chainId != it->chainId) {
                throw Exception(
                    fmt::format("Chain ID '{}' of atom {} does not match chain ID '{}' of residue",
                                chainId, atomToStr(i), it->chainId),
                    IVW_CONTEXT);
            }
            // add atom to residue
            residueAtoms_[{resId, chainId}].push_back(i);

            const auto residueIndex = std::distance(data_.residues.begin(), it);
            residueIndices_.push_back(residueIndex);
        }

        if (!data_.chains.empty()) {
            // update chain information
            for (auto&& [residueIndex, res] : util::enumerate(data_.residues)) {
                auto chainIt = util::find_if(
                    data_.chains, [id = res.chainId](const Chain& c) { return c.id == id; });
                if (chainIt == data_.chains.end()) {
                    throw Exception(fmt::format("Invalid chain ID '{}' in residue {} '{}'",
                                                res.chainId, res.id, res.name));
                }
                chainResidues_[res.chainId].push_back(residueIndex);
            }
        }
    }
}

const MolecularData& MolecularStructure::data() const { return data_; }

const Atoms& MolecularStructure::atoms() const { return data_.atoms; }

const std::vector<Residue>& MolecularStructure::residues() const { return data_.residues; }

const std::vector<Chain>& MolecularStructure::chains() const { return data_.chains; }

const std::vector<Bond>& MolecularStructure::bonds() const { return data_.bonds; }

std::optional<size_t> MolecularStructure::getAtomIndex(const std::string& fullAtomName,
                                                       size_t residueId, size_t chainId) const {
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

bool MolecularStructure::hasResidues() const { return !residueIndices_.empty(); }

bool MolecularStructure::hasResidue(size_t residueId, size_t chainId) const {
    return residueAtoms_.find({residueId, chainId}) != residueAtoms_.end();
}

bool MolecularStructure::hasChains() const { return !chainResidues_.empty(); }

bool MolecularStructure::hasChain(size_t chainId) const {
    return chainResidues_.find(chainId) != chainResidues_.end();
}

const std::vector<size_t>& MolecularStructure::getResidueAtoms(size_t residueId,
                                                               size_t chainId) const {
    if (auto it = residueAtoms_.find({residueId, chainId}); it != residueAtoms_.end()) {
        return it->second;
    } else {
        throw Exception(fmt::format("Residue with ID '{}' and chain ID '{}' does not exist",
                                    residueId, chainId),
                        IVW_CONTEXT);
    }
}

const std::vector<size_t>& MolecularStructure::getChainResidues(size_t chainId) const {
    if (auto it = chainResidues_.find(chainId); it != chainResidues_.end()) {
        return it->second;
    } else {
        throw Exception(fmt::format("Chain with chain ID '{}' does not exist", chainId),
                        IVW_CONTEXT);
    }
}

const std::vector<size_t>& MolecularStructure::getResidueIndices() const { return residueIndices_; }

void MolecularStructure::verifyData() const {
    const size_t atomCount = data_.atoms.positions.size();

    // attributes of atoms in MolecularData can be either empty or must match the number of atoms
    auto sizeValid = [atomCount](const auto& v) { return (v.empty() || (v.size() == atomCount)); };

    if (!sizeValid(data_.atoms.bFactors)) {
        throw Exception(fmt::format("Inconsistent MolecularData: expected {} b Factors, found {}",
                                    atomCount, data_.atoms.bFactors.size()),
                        IVW_CONTEXT);
    }
    if (!sizeValid(data_.atoms.modelIds)) {
        throw Exception(fmt::format("Inconsistent MolecularData: expected {} model IDs, found {}",
                                    atomCount, data_.atoms.modelIds.size()),
                        IVW_CONTEXT);
    }
    if (!sizeValid(data_.atoms.chainIds)) {
        throw Exception(fmt::format("Inconsistent MolecularData: expected {} chain IDs, found {}",
                                    atomCount, data_.atoms.chainIds.size()),
                        IVW_CONTEXT);
    }
    if (!sizeValid(data_.atoms.residueIds)) {
        throw Exception(fmt::format("Inconsistent MolecularData: expected {} residue Ids, found {}",
                                    atomCount, data_.atoms.residueIds.size()),
                        IVW_CONTEXT);
    }
    if (!sizeValid(data_.atoms.atomicNumbers)) {
        throw Exception(
            fmt::format("Inconsistent MolecularData: expected {} atomic numbers, found {}",
                        atomCount, data_.atoms.atomicNumbers.size()),
            IVW_CONTEXT);
    }
    if (!sizeValid(data_.atoms.fullNames)) {
        throw Exception(fmt::format("Inconsistent MolecularData: expected {} full names, found {}",
                                    atomCount, data_.atoms.fullNames.size()),
                        IVW_CONTEXT);
    }
}

}  // namespace molvis

}  // namespace inviwo
