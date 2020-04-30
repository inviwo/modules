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
#pragma once

#include <inviwo/molvisbase/molvisbasemoduledefine.h>
#include <inviwo/core/util/glmvec.h>

#include <inviwo/molvisbase/datastructures/molecularstructure.h>

namespace inviwo {

class Mesh;

namespace molvis {

/**
 * find a residue in \p data matching both \p residueId and \p chainId.
 *
 * @param data   only considers the list of residues
 * @return matching residue if found, std::nullopt otherwise
 */
std::optional<Residue> IVW_MODULE_MOLVISBASE_API findResidue(const MolecularData& data,
                                                             size_t residueId, size_t chainId);

/**
 * find a chain in \p data matching the \p chainId.
 *
 * @param data   only considers the list of chains
 * @return matching residue if found, std::nullopt otherwise
 */
std::optional<Chain> IVW_MODULE_MOLVISBASE_API findChain(const MolecularData& data, size_t chainId);

/**
 * get the global index of an atom in \p atoms. Both \p fullAtomName, \p residueId, and \p chainId
 * must match.
 *
 * @param data   requires chainIds, residueIds, and fullNames
 * @return global atom index if found, std::nullopt otherwise
 * @throws Exception if chainIds, residueIds, and fullNames in atoms have different sizes
 */
std::optional<size_t> IVW_MODULE_MOLVISBASE_API getGlobalAtomIndex(const Atoms& atoms,
                                                                   const std::string& fullAtomName,
                                                                   size_t residueId,
                                                                   size_t chainId);

/**
 * Determine covalent bonds based on heuristics. A bond is valid if the distance between two
 * atoms lies in between (r_1 + r_2 - 0.5) and (r_1 + r_2 + 0.3) where r_i is the covalent radius of
 * atom i.
 *
 * @param atoms  requires only atom positions and atomic numbers
 * @return list of covalent bonds
 * @throws Exception if sizes of positions and atomic numbers do not match
 */
std::vector<Bond> IVW_MODULE_MOLVISBASE_API computeCovalentBonds(const Atoms& atoms);

/**
 * Determines the atomic numbers of each atom based on the respective full atom names. This function
 * requires the full names to be set.
 *
 * @param atoms   only considers full names of atoms
 * @return list of atomic numbers matching the full names of the input
 *
 * \see element::fromFullName()
 */
std::vector<unsigned char> IVW_MODULE_MOLVISBASE_API getAtomicNumbers(const Atoms& atoms);

/**
 * \brief creates a mesh from molecular structure \p s
 *
 * Atoms are encoded as points with a radius attribute. Colors as well as van der Waals radii are
 * obtained for each atom using its atomic number. Bonds between atoms are represented as lines.
 *
 * \see MolecularStructure, element::color, element::vdwRadius
 */
std::shared_ptr<Mesh> IVW_MODULE_MOLVISBASE_API createMesh(const MolecularStructure& s);

}  // namespace molvis

}  // namespace inviwo
