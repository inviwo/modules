/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2023 Inviwo Foundation
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

#include <inviwo/molvisbase/util/atomicelement.h>
#include <inviwo/molvisbase/util/aminoacid.h>

#include <optional>
#include <iostream>
#include <unordered_map>
#include <string_view>
#include <fmt/format.h>

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
    std::vector<int> serialNumbers;
    std::vector<double> bFactors;  //<!  Debye-Waller factor (DWF) or temperature factor.
    std::vector<int> modelIds;
    std::vector<int> chainIds;
    std::vector<int> residueIds;
    std::vector<Element> atomicNumbers;
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
    int id;
    AminoAcid aminoacid;
    std::string fullName;  //!< hetero flag 'H_' plus hetero residue or 'W' for water molecules,
                           //!< sequence identifier, and insertion code
    int chainId;
};

/**
 * \brief represents a Chain of a molecular structure
 *
 * A chain has a unique identifier and name and refers to a list of all comprised residues.
 *
 * \see https://biopython.org/wiki/The_Biopython_Structural_Bioinformatics_FAQ
 */
struct IVW_MODULE_MOLVISBASE_API Chain {
    int id;
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

enum class PeptideType { Unknown, General, Glycine, Proline, PrePro };

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ss, PeptideType pt) {
    switch (pt) {
        case PeptideType::Glycine:
            ss << "Glycine";
            break;
        case PeptideType::Proline:
            ss << "Proline";
            break;
        case PeptideType::PrePro:
            ss << "Pre-Pro";
            break;
        case PeptideType::General:
            ss << "General";
            break;
        case PeptideType::Unknown:
        default:
            ss << "Unknown";
            break;
    }
    return ss;
}

}  // namespace molvis
}  // namespace inviwo

template <>
struct fmt::formatter<inviwo::molvis::PeptideType> : fmt::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(inviwo::molvis::PeptideType c, FormatContext& ctx) {
        string_view name = "Unknown";
        switch (c) {
            case inviwo::molvis::PeptideType::Glycine:
                name = "Glycine";
                break;
            case inviwo::molvis::PeptideType::Proline:
                name = "Proline";
                break;
            case inviwo::molvis::PeptideType::PrePro:
                name = "Pre-Pro";
                break;
            case inviwo::molvis::PeptideType::General:
                name = "General";
                break;
            case inviwo::molvis::PeptideType::Unknown:
            default:
                break;
        }
        return formatter<string_view>::format(name, ctx);
    }
};

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
     * \brief data structure defining a single backbone segment
     *
     * Data structure holding the atom indices of a single backbone segment including
     * dihedral angles.
     * A backbone segment may be incomplete if any of the atoms (C, O, N, or Ca) is
     * missing. Dihedral angles are only valid if the segment is complete.
     */
    struct BackboneSegment {
        constexpr bool complete() const noexcept {
            return ca.has_value() && n.has_value() && c.has_value() && o.has_value();
        }
        constexpr bool empty() const noexcept {
            return !ca.has_value() && !n.has_value() && !c.has_value() && !o.has_value();
        }

        int resId;
        int chainId;

        std::optional<size_t> ca;
        std::optional<size_t> n;
        std::optional<size_t> c;
        std::optional<size_t> o;

        // dihedral angles
        std::optional<double> phi;
        std::optional<double> psi;
        PeptideType type = PeptideType::General;
    };

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

    const MolecularData& data() const;
    const Atoms& atoms() const;
    const std::vector<Residue>& residues() const;
    const std::vector<Chain>& chains() const;
    const std::vector<Bond>& bonds() const;

    /**
     * get the global index of an atom. Both \p fullAtomName, \p residueId, and \p chainId
     * must match. This function will in many cases be faster than getGlobalAtomIndex() since the
     * atom is located through its residue.
     *
     * @return global atom index if found, std::nullopt otherwise
     *
     * \see getGlobalAtomIndex
     */
    std::optional<size_t> getAtomIndex(std::string_view fullAtomName, int residueId,
                                       int chainId) const;

    bool hasAtoms() const;
    bool hasResidues() const;
    bool hasResidue(int residueId, int chainId) const;
    bool hasChains() const;
    bool hasChain(int chainId) const;

    /**
     * query a residue matching \p residueId and \p chainId for its atoms.
     *
     * @return list of atom indices which belong to the residue
     * @throws Exception if residue does not exist
     */
    const std::vector<size_t>& getResidueAtoms(int residueId, int chainId) const;

    /**
     * query a chain matching \p chainId for its residues.
     *
     * @return list of residue indices which belong to the chain
     * @throws Exception if chain does not exist
     */
    const std::vector<size_t>& getChainResidues(int chainId) const;

    /**
     * query a chain matching \p chainId for its backbone segments.
     *
     * @return list of backbone segments which belong to the chain
     * @throws Exception if chain does not exist
     */
    const std::vector<BackboneSegment>& getBackboneSegments(int chainId) const;

    /**
     * returns an index for each atom referring to its parent residue. This provides faster access
     * to residues through indexing instead of locating a matching residue by ID. The returned list
     * might be empty if residue information is not available.
     *
     * @return list of per-atom residue indices
     */
    const std::vector<size_t>& getResidueIndices() const;

    /**
     * returns an index for each atom referring to its parent backbone segment. This provides faster
     * access to backbone segments through indexing instead of locating a matching backbone by
     * residue and chain ID. The returned list might be empty if residue and chain information is
     * not available.
     *
     * @return list of per-atom backbone indices
     */
    const std::vector<size_t>& getBackboneSegmentIndices() const;

private:
    MolecularData data_;

    // mapping chain IDs to a list of global residue indices
    std::unordered_map<int, std::vector<size_t>> chainResidues_;
    // mapping residue IDs to a list of global atom indices
    std::unordered_map<ResidueID, std::vector<size_t>> residueAtoms_;
    // mapping residue IDs to global residue indices
    std::unordered_map<ResidueID, size_t> residueIndices_;
    // residue index of each atom
    std::vector<size_t> atomResidueIndices_;

    // mapping chain IDs to a list of backbone segments
    std::unordered_map<int, std::vector<BackboneSegment>> chainSegments_;
    // backbone segment index of each atom
    std::vector<size_t> chainSegmentIndices_;
};

}  // namespace molvis

}  // namespace inviwo
