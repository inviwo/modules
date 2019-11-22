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

#include <ivwmolvis/pymolvis.h>

#include <warn/push>
#include <warn/ignore/shadow>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <warn/pop>

#include <inviwo/molvisbase/datastructures/molecularstructure.h>
#include <modules/python3/pyportutils.h>

#include <fmt/format.h>

namespace py = pybind11;

namespace inviwo {

using namespace molvis;

void exposeMolVis(pybind11::module& m) {

    py::class_<Atoms, std::shared_ptr<Atoms>>(m, "Atoms")
        .def(py::init())
        .def("updateAtomNumbers", &Atoms::updateAtomNumbers)
        .def_readwrite("positions", &Atoms::positions)
        //.def_readwrite("structureids", &Atoms::structureIds)
        .def_readwrite("modelids", &Atoms::modelIds)
        .def_readwrite("chainids", &Atoms::chainIds)
        .def_readwrite("residueids", &Atoms::residueIds)
        .def_readwrite("atomnumbers", &Atoms::atomNumbers)
        .def_readwrite("fullnames", &Atoms::fullNames)
        .def("__repr__",
             [](Atoms& a) { return fmt::format("<Atoms: {} entries>", a.atomNumbers.size()); });

    py::class_<Residue, std::shared_ptr<Residue>>(m, "Residue")
        .def(py::init())
        .def_readwrite("id", &Residue::id)
        .def_readwrite("name", &Residue::name)
        .def_readwrite("fullname", &Residue::fullName)
        .def_readwrite("chainid", &Residue::chainId)
        .def_readwrite("atoms", &Residue::atoms)
        .def("__repr__", [](Residue& r) {
            return fmt::format("<Residue: {}, '{}', '{}', {}, {} atoms>", r.id, r.name, r.fullName,
                               r.chainId, r.atoms.size());
        });

    py::class_<Chain, std::shared_ptr<Chain>>(m, "Chain")
        .def(py::init())
        .def_readwrite("id", &Chain::id)
        .def_readwrite("name", &Chain::name)
        .def_readwrite("residues", &Chain::residues)
        .def("__repr__", [](Chain& c) {
            return fmt::format("<Chain: {}, '{}', {} residues>", c.id, c.name, c.residues.size());
        });

    py::class_<MolecularStructure, std::shared_ptr<MolecularStructure>>(m, "MolecularStructure")
        .def(py::init())
        //.def(py::init<std::uint32_t>(), py::arg("size") = 0)
        .def_property_readonly("atomcount", &MolecularStructure::getAtomCount)
        .def_property_readonly("residuecount", &MolecularStructure::getResidueCount)
        .def_property_readonly("chaincount", &MolecularStructure::getChainCount)

        .def_property("atoms", &MolecularStructure::getAtoms, &MolecularStructure::setAtoms)
        .def_property("residues", &MolecularStructure::getResidues,
                      &MolecularStructure::setResidues)
        .def_property("chains", &MolecularStructure::getChains, &MolecularStructure::setChains)

        .def("getResidue", &MolecularStructure::getResidue)
        .def("getChain", &MolecularStructure::getChain)
        .def("getAtomIndex", &MolecularStructure::getAtomIndex)

        .def("__repr__", [](const MolecularStructure& s) {
            std::string str =
                fmt::format("<MolecularStructure: {} atom(s), {} residue(s), {} chain(s)",
                            s.getAtomCount(), s.getResidueCount(), s.getChainCount());
            return str + ">";
        });

    exposeStandardDataPorts<MolecularStructure>(m, "MolecularStructure");
}

}  // namespace inviwo
