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
#pragma once

#include <inviwo/molvisbase/molvisbasemoduledefine.h>

#include <inviwo/molvisbase/datastructures/molecularstructure.h>
#include <inviwo/core/util/document.h>

#include <string_view>

namespace inviwo {

class Mesh;

namespace molvis {

/**
 * find a residue in \p data matching both \p residueId and \p chainId.
 *
 * @param data   only considers the list of residues
 * @return matching residue if found, std::nullopt otherwise
 */
IVW_MODULE_MOLVISBASE_API std::optional<Residue> findResidue(const MolecularData& data,
                                                             int residueId, int chainId);

/**
 * find a chain in \p data matching the \p chainId.
 *
 * @param data   only considers the list of chains
 * @return matching residue if found, std::nullopt otherwise
 */
IVW_MODULE_MOLVISBASE_API std::optional<Chain> findChain(const MolecularData& data, int chainId);

/**
 * get the global index of an atom in \p atoms. Both \p fullAtomName, \p residueId, and \p chainId
 * must match.
 *
 * @param data   requires chainIds, residueIds, and fullNames
 * @return global atom index if found, std::nullopt otherwise
 * @throws Exception if chainIds, residueIds, and fullNames in atoms have different sizes
 */
IVW_MODULE_MOLVISBASE_API std::optional<size_t> getGlobalAtomIndex(const Atoms& atoms,
                                                                   std::string_view fullAtomName,
                                                                   int residueId, int chainId);

/**
 * determine peptide type of a residue
 *
 * @param residue      type of the residue
 * @param nextResidue  type of next next residue
 * @return type of peptide
 */
IVW_MODULE_MOLVISBASE_API PeptideType getPeptideType(AminoAcid residue, AminoAcid nextResidue);

/**
 * determine peptide type of a residue
 *
 * @param resName      name of the residue ("GLY", "PRO", ...)
 * @param nextResName  name of next residue ("GLY", "PRO", ...)
 * @return type of peptide
 */
IVW_MODULE_MOLVISBASE_API PeptideType getPeptideType(std::string_view resName,
                                                     std::string_view nextResName);

/**
 * Determine covalent bonds based on heuristics. A bond is valid if the distance between two
 * atoms lies in between (r_1 + r_2 - 0.5) and (r_1 + r_2 + 0.3) where r_i is the covalent radius of
 * atom i.
 *
 * @param atoms  requires only atom positions and atomic numbers
 * @return list of covalent bonds
 * @throws Exception if sizes of positions and atomic numbers do not match
 */
IVW_MODULE_MOLVISBASE_API std::vector<Bond> computeCovalentBonds(const Atoms& atoms);

/**
 * Determines the atomic numbers of each atom based on the respective \p fullNames.
 *
 * @return list of atomic numbers matching the full names of the input
 *
 * \see element::fromFullName()
 */
IVW_MODULE_MOLVISBASE_API std::vector<Element> getAtomicNumbers(
    const std::vector<std::string>& fullNames);

/**
 * \brief creates a mesh from molecular structure \p s
 *
 * Atoms are encoded as points with a radius attribute. Colors as well as van der Waals radii are
 * obtained for each atom using its atomic number. Bonds between atoms are represented as lines.
 *
 * @param s               molecular structure
 * @param enablePicking   if true, the picking attribute will be added to the mesh. Ids are assigned
 *                        in the same order as the atoms/positions in s
 * @param startId    global picking ID for first atom (i.e. PickingMapper::getPickingId(0)), do not
 *                   forget to resize the PickingMapper to the number of atoms
 * @return mesh containing the molecular structure
 *
 * \see MolecularStructure, element::color, element::vdwRadius
 */
IVW_MODULE_MOLVISBASE_API std::shared_ptr<Mesh> createMesh(const MolecularStructure& s,
                                                           bool enablePicking = false,
                                                           uint32_t startId = 0);

/**
 * \brief create a tool tip for the given \p atom of molecular structure \p s
 *
 * @param s      molecular structure
 * @param atom   index of the atom
 * @return Document containing tool tip with information on atomic element, residue, chain, etc.
 */
IVW_MODULE_MOLVISBASE_API Document createToolTip(const MolecularStructure& s, int atomIndex);

}  // namespace molvis

}  // namespace inviwo
