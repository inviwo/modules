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

#include <inviwo/molvisbase/datastructures/molecularstructure.h>
#include <inviwo/core/util/stdextensions.h>

#include <inviwo/molvisbase/util/molvisutils.h>

namespace inviwo {

namespace molvis {

void Atoms::updateAtomNumbers() {
    atomNumbers = util::transform(fullNames, [](const std::string& name) {
        return static_cast<unsigned char>(molvis::element::fromFullName(name));
    });
}

bool Atoms::empty() const { return positions.empty(); }

size_t Atoms::size() const { return positions.size(); }

void Atoms::clear() {
    positions.clear();
    // structureIds.clear();
    modelIds.clear();
    chainIds.clear();
    residueIds.clear();
    atomNumbers.clear();
    fullNames.clear();

    residueIndices.clear();
}

MolecularStructure::MolecularStructure() = default;

size_t MolecularStructure::getAtomCount() const { return atoms_.positions.size(); }

size_t MolecularStructure::getResidueCount() const { return residues_.size(); }

size_t MolecularStructure::getChainCount() const { return chains_.size(); }

void MolecularStructure::setAtoms(Atoms atoms) { atoms_ = std::move(atoms); }

void MolecularStructure::setResidues(std::vector<Residue> residues) {
    residues_ = std::move(residues);
}

void MolecularStructure::setChains(std::vector<Chain> chains) { chains_ = std::move(chains); }

const Atoms& MolecularStructure::getAtoms() const { return atoms_; }

const std::vector<Residue>& MolecularStructure::getResidues() const { return residues_; }

const std::vector<Chain>& MolecularStructure::getChains() const { return chains_; }

std::optional<Residue> MolecularStructure::getResidue(size_t residueId, size_t chainId) const {
    auto it = util::find_if(residues_,
                            [&](auto& r) { return (r.id == residueId) && (r.chainId == chainId); });
    if (it != residues_.end()) {
        return *it;
    } else {
        return std::nullopt;
    }
}

std::optional<Chain> MolecularStructure::getChain(size_t chainId) const {
    auto it = util::find_if(chains_, [id = chainId](auto& r) { return r.id == id; });
    if (it != chains_.end()) {
        return *it;
    } else {
        return std::nullopt;
    }
}

std::optional<size_t> MolecularStructure::getAtomIndex(const std::string& fullAtomName,
                                                       size_t residueId, size_t chainId) const {
    auto residue = getResidue(residueId, chainId);
    if (residue) {
        for (auto i : residue->atoms) {
            if ((atoms_.fullNames[i] == fullAtomName) && (atoms_.residueIds[i] == residueId) &&
                (atoms_.chainIds[i] == chainId)) {
                return i;
            }
        }
    } else {
        for (size_t i = 0; i < atoms_.fullNames.size(); ++i) {
            if ((atoms_.fullNames[i] == fullAtomName) && (atoms_.residueIds[i] == residueId) &&
                (atoms_.chainIds[i] == chainId)) {
                return i;
            }
        }
    }
    return std::nullopt;
}

void MolecularStructure::updateStructure() {
    atoms_.residueIndices.clear();

    for (size_t i = 0; i < atoms_.size(); ++i) {
        const auto resId = atoms_.residueIds[i];
        const auto chainId = atoms_.chainIds[i];
        auto it = util::find_if(residues_, [id = resId](const Residue& r) { return r.id == id; });
        if (it == residues_.end()) {
            throw Exception(fmt::format("Invalid residue ID '{}'", resId), IVW_CONTEXT);
        }
        IVW_ASSERT(chainId == it->chainId, "Chain ID mismatch");

        it->atoms.insert(i);
        const auto residueIndex = std::distance(residues_.begin(), it);
        atoms_.residueIndices.push_back(static_cast<int>(residueIndex));

        auto chainIt =
            util::find_if(chains_, [id = chainId](const Chain& c) { return c.id == id; });
        if (chainIt == chains_.end()) {
            throw Exception(fmt::format("Invalid chain ID '{}'", chainId));
        }
        chainIt->residues.insert(residueIndex);
    }
}

}  // namespace molvis

}  // namespace inviwo
