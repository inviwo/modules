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
#pragma once

#include <inviwo/molvisbase/molvisbasemoduledefine.h>
#include <inviwo/core/util/glmvec.h>
#include <inviwo/core/util/hashcombine.h>

#include <optional>

namespace inviwo {

namespace molvis {

/**
 * \brief represents all atoms of a molecular structure
 *
 * This layout is loosely based on the atom data structures used in BioPython.
 * This structure has no invariants. Invariants are only checked by various helper and utility
 * functions.
 *
 * \see https://biopython.org/wiki/The_Biopython_Structural_Bioinformatics_FAQ
 */
struct IVW_MODULE_MOLVISBASE_API Atoms {
    std::vector<dvec3> positions;
    std::vector<double> bFactors;  //<!  Debye-Waller factor (DWF) or temperature factor.
    std::vector<int> modelIds;
    std::vector<int> chainIds;
    std::vector<int> residueIds;
    std::vector<unsigned char> atomicNumbers;
    std::vector<std::string>
        fullNames;  //!< full descriptive name, might include spaces and additional identifiers
};

/**
 * \brief represents a Residue of a molecular structure
 *
 * A residue has a unique identifier and name. It refers to a list of all atoms belonging to this
 * residue. The full name contains additional information.
 *
 * \see https://biopython.org/wiki/The_Biopython_Structural_Bioinformatics_FAQ
 */
struct IVW_MODULE_MOLVISBASE_API Residue {
    size_t id;
    std::string name;
    std::string fullName;  //!< hetero flag 'H_' plus hetero residue or 'W' for water molecules,
                           //!< sequence identifier, and insertion code
    size_t chainId;
};

/**
 * \brief represents a Chain of a molecular structure
 *
 * A chain has a unique identifier and name and refers to a list of all comprised residues.
 *
 * \see https://biopython.org/wiki/The_Biopython_Structural_Bioinformatics_FAQ
 */
struct IVW_MODULE_MOLVISBASE_API Chain {
    size_t id;
    std::string name;
};

/**
 * represents a bond between two atoms and refers to the atom indices
 */
using Bond = std::pair<size_t, size_t>;

/**
 * \brief represents molecular data consisting of atoms and optional residue and chain information
 * as well as bonds
 *
 * A MolecularData object requires the atoms to provide at least positions to be considered a valid
 * molecular structure. Residue and chain information is optional. Information on bonds between
 * atoms is also optional.
 *
 * Molecular data can consists of multiple chains. Chains consist of one or several residues which
 * in turn contain individual atoms. This data structure assumes that only atoms refer to residues
 * and chains via IDs. The hierarchical information is extracted in and used by MolecularStructure.
 *
 * \see MolecularStructure
 */
struct IVW_MODULE_MOLVISBASE_API MolecularData {
    std::optional<std::string> source;

    Atoms atoms;
    std::vector<Residue> residues;
    std::vector<Chain> chains;
    std::vector<Bond> bonds;
};

/// residue's ID and chain ID for unique identification
using ResidueID = std::pair<size_t, size_t>;

}  // namespace molvis
}  // namespace inviwo

namespace std {
template <>
struct hash<inviwo::molvis::ResidueID> {
    size_t operator()(const inviwo::molvis::ResidueID& resId) const {
        size_t h = 0;
        inviwo::util::hash_combine(h, resId.first);
        inviwo::util::hash_combine(h, resId.second);
        return h;
    }
};
}  // namespace std

namespace inviwo {
namespace molvis {

/**
 * \brief a data structure holding molecular data and its acceleration structures
 *
 * A MolecularStructure holds and provides access to the hierarchical information of molecular data,
 * i.e. chains, residues, and atoms, if existing. Acceleration structures are used for faster
 * lookups of, e.g. querying all atoms in a particular residue.
 *
 * Note: acceleration structures can only be created if the molecular data provides information on
 * residues or both residues and chains.
 *
 * \see MolecularData
 */
class IVW_MODULE_MOLVISBASE_API MolecularStructure {
public:
    /**
     * \brief create a MolecularStructure from \p data
     *
     * Performs some basic sanity checks like ID consistency, equal size of atom attributes
     * (positions, IDs, atomic numbers, etc.) and builds acceleration structures.
     *
     * @throws Exception if atoms refer to invalid residues/chains, an atom's chain ID mismatches
     *         the chain ID of the residue, or if attributes stored in atom have different sizes
     *         (empty attributes are ignored).
     */
    MolecularStructure(MolecularData data);
    MolecularStructure() = delete;
    ~MolecularStructure() = default;

    const MolecularData& get() const;

    /**
     * get the global index of an atom. Both \p fullAtomName, \p residueId, and \p chainId
     * must match. This function will in many cases be faster than getGlobalAtomIndex() since the
     * atom is located through its residue.
     *
     * @return global atom index if found, std::nullopt otherwise
     *
     * \see getGlobalAtomIndex
     */
    std::optional<size_t> getAtomIndex(const std::string& fullAtomName, size_t residueId,
                                       size_t chainId) const;

    bool hasResidues() const;
    bool hasResidue(size_t residueId, size_t chainId) const;
    bool hasChains() const;
    bool hasChain(size_t chainId) const;

    /**
     * query a residue matching \p residueId and \p chainId for its atoms.
     *
     * @return list of atom indices which belong to the residue
     * @throws Exception if residue does not exist
     */
    const std::vector<size_t>& getResidueAtoms(size_t residueId, size_t chainId) const;

    /**
     * query a chain matching \p chainId for its residues.
     *
     * @return list of residue indices which belong to the chain
     * @throws Exception if chain does not exist
     */
    const std::vector<size_t>& getChainResidues(size_t chainId) const;

    /**
     * returns an index for each atom referring to its parent residue. This provides faster access
     * to residues through indexing instead of locating a matching residue by ID. The returned list
     * might be empty if residue information is not available.
     *
     * @return list of per-atom residue indices
     */
    const std::vector<size_t>& getResidueIndices() const;

private:
    void verifyData() const;

    MolecularData data_;

    // mapping chain IDs to a list of global residue indices
    std::unordered_map<size_t, std::vector<size_t>> chainResidues_;
    // mapping residue IDs to a list of global atom indices
    std::unordered_map<ResidueID, std::vector<size_t>> residueAtoms_;
    // residue index of each atom
    std::vector<size_t> residueIndices_;
};

}  // namespace molvis

}  // namespace inviwo
