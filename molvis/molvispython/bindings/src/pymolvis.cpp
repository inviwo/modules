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

#include <ivwmolvis/pymolvis.h>

#include <warn/push>
#include <warn/ignore/shadow>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <warn/pop>

#include <inviwo/core/datastructures/geometry/mesh.h>

#include <inviwo/molvisbase/datastructures/molecularstructure.h>
#include <inviwo/molvisbase/datastructures/molecularstructuretraits.h>
#include <inviwo/molvisbase/util/molvisutils.h>
#include <inviwo/molvisbase/util/atomicelement.h>
#include <modules/python3/pyportutils.h>

#include <fmt/format.h>

namespace py = pybind11;

namespace inviwo {

using namespace molvis;

namespace detail {

void exposeAtomicElement(pybind11::module& m) {
    m.def("element", element::element, py::arg("atomicNumber"))
        .def("name", element::name, py::arg("symbol"))
        .def("symbol", element::symbol, py::arg("symbol"))
        .def("color", element::color, py::arg("symbol"))
        .def("vdwRadius", element::vdwRadius, py::arg("symbol"))
        .def("covalentRadius", element::covalentRadius, py::arg("symbol"))
        .def("atomicMass", element::atomicMass, py::arg("symbol"))
        .def("fromFullName", element::fromFullName, py::arg("fullAtomName"));

    py::enum_<Element>(m, "Element")
        .value("Unknown", Element::Unknown)
        .value("H", Element::H)
        .value("He", Element::He)
        .value("Li", Element::Li)
        .value("Be", Element::Be)
        .value("B", Element::B)
        .value("C", Element::C)
        .value("N", Element::N)
        .value("O", Element::O)
        .value("F", Element::F)
        .value("Ne", Element::Ne)
        .value("Na", Element::Na)
        .value("Mg", Element::Mg)
        .value("Al", Element::Al)
        .value("Si", Element::Si)
        .value("P", Element::P)
        .value("S", Element::S)
        .value("Cl", Element::Cl)
        .value("Ar", Element::Ar)
        .value("K", Element::K)
        .value("Ca", Element::Ca)
        .value("Sc", Element::Sc)
        .value("Ti", Element::Ti)
        .value("V", Element::V)
        .value("Cr", Element::Cr)
        .value("Mn", Element::Mn)
        .value("Fe", Element::Fe)
        .value("Co", Element::Co)
        .value("Ni", Element::Ni)
        .value("Cu", Element::Cu)
        .value("Zn", Element::Zn)
        .value("Ga", Element::Ga)
        .value("Ge", Element::Ge)
        .value("As", Element::As)
        .value("Se", Element::Se)
        .value("Br", Element::Br)
        .value("Kr", Element::Kr)
        .value("Rb", Element::Rb)
        .value("Sr", Element::Sr)
        .value("Y", Element::Y)
        .value("Zr", Element::Zr)
        .value("Nb", Element::Nb)
        .value("Mo", Element::Mo)
        .value("Tc", Element::Tc)
        .value("Ru", Element::Ru)
        .value("Rh", Element::Rh)
        .value("Pd", Element::Pd)
        .value("Ag", Element::Ag)
        .value("Cd", Element::Cd)
        .value("In", Element::In)
        .value("Sn", Element::Sn)
        .value("Sb", Element::Sb)
        .value("Te", Element::Te)
        .value("I", Element::I)
        .value("Xe", Element::Xe)
        .value("Cs", Element::Cs)
        .value("Ba", Element::Ba)
        .value("La", Element::La)
        .value("Ce", Element::Ce)
        .value("Pr", Element::Pr)
        .value("Nd", Element::Nd)
        .value("Pm", Element::Pm)
        .value("Sm", Element::Sm)
        .value("Eu", Element::Eu)
        .value("Gd", Element::Gd)
        .value("Tb", Element::Tb)
        .value("Dy", Element::Dy)
        .value("Ho", Element::Ho)
        .value("Er", Element::Er)
        .value("Tm", Element::Tm)
        .value("Yb", Element::Yb)
        .value("Lu", Element::Lu)
        .value("Hf", Element::Hf)
        .value("Ta", Element::Ta)
        .value("W", Element::W)
        .value("Re", Element::Re)
        .value("Os", Element::Os)
        .value("Ir", Element::Ir)
        .value("Pt", Element::Pt)
        .value("Au", Element::Au)
        .value("Hg", Element::Hg)
        .value("Tl", Element::Tl)
        .value("Pb", Element::Pb)
        .value("Bi", Element::Bi)
        .value("Po", Element::Po)
        .value("At", Element::At)
        .value("Rn", Element::Rn)
        .value("Fr", Element::Fr)
        .value("Ra", Element::Ra)
        .value("Ac", Element::Ac)
        .value("Th", Element::Th)
        .value("Pa", Element::Pa)
        .value("U", Element::U)
        .value("Np", Element::Np)
        .value("Pu", Element::Pu)
        .value("Am", Element::Am)
        .value("Cm", Element::Cm)
        .value("Bk", Element::Bk)
        .value("Cf", Element::Cf)
        .value("Es", Element::Es)
        .value("Fm", Element::Fm)
        .value("Md", Element::Md)
        .value("No", Element::No)
        .value("Lr", Element::Lr)
        .value("Rf", Element::Rf)
        .value("Db", Element::Db)
        .value("Sg", Element::Sg)
        .value("Bh", Element::Bh)
        .value("Hs", Element::Hs)
        .value("Mt", Element::Mt)
        .value("Ds", Element::Ds)
        .value("Rg", Element::Rg)
        .value("Cn", Element::Cn)
        .value("Nh", Element::Nh)
        .value("Fl", Element::Fl)
        .value("Mc", Element::Mc)
        .value("Lv", Element::Lv)
        .value("Ts", Element::Ts)
        .value("Og", Element::Og);
}

void exposeMolVisUtil(pybind11::module& m) {
    m.def("findResidue", &findResidue, py::arg("data"), py::arg("residueId"), py::arg("chainId"))
        .def("findChain", &findChain, py::arg("data"), py::arg("chainId"))
        .def("getGlobalAtomIndex", &getGlobalAtomIndex, py::arg("atoms"), py::arg("fullAtomName"),
             py::arg("residueId"), py::arg("chainId"))
        .def("computeCovalentBonds", &computeCovalentBonds, py::arg("atoms"))
        .def("getAtomicNumbers", &getAtomicNumbers, py::arg("atoms"))
        .def("createMesh", &createMesh, py::arg("structure"));
}

}  // namespace detail

void exposeMolVis(pybind11::module& m) {
    auto mAtomicElement =
        m.def_submodule("atomicelement", "Atomic element lookup tables and functions for MolVis");
    detail::exposeAtomicElement(mAtomicElement);
    auto mUtil = m.def_submodule("util", "MolVis utils");
    detail::exposeMolVisUtil(mUtil);

    py::class_<Atoms, std::shared_ptr<Atoms>>(m, "Atoms")
        .def(py::init())
        .def_readwrite("positions", &Atoms::positions)
        .def_readwrite("bfactors", &Atoms::bFactors)
        .def_readwrite("modelids", &Atoms::modelIds)
        .def_readwrite("chainids", &Atoms::chainIds)
        .def_readwrite("residueids", &Atoms::residueIds)
        .def_readwrite("atomicnumbers", &Atoms::atomicNumbers)
        .def_readwrite("fullnames", &Atoms::fullNames)
        .def("__len__", [](Atoms& a) { return a.positions.size(); })
        .def("__repr__", [](Atoms& a) {
            return fmt::format(
                "<Atoms: {} positions, {} b Factors, {} model IDs, {} chain IDs, {} residue IDs, "
                "{} atomic numbers, {} full names>",
                a.positions.size(), a.bFactors.size(), a.modelIds.size(), a.chainIds.size(),
                a.residueIds.size(), a.atomicNumbers.size(), a.fullNames.size());
        });

    py::class_<Residue, std::shared_ptr<Residue>>(m, "Residue")
        .def(py::init())
        .def(py::init([](size_t id, const std::string& name, const std::string& fullname,
                         size_t chainid) -> Residue {
                 return {id, name, fullname, chainid};
             }),
             py::arg("id"), py::arg("name"), py::arg("fullname"), py::arg("chainid"))
        .def_readwrite("id", &Residue::id)
        .def_readwrite("name", &Residue::name)
        .def_readwrite("fullname", &Residue::fullName)
        .def_readwrite("chainid", &Residue::chainId)
        .def("__repr__", [](Residue& r) {
            return fmt::format("<Residue: {}, '{}', '{}', {}>", r.id, r.name, r.fullName,
                               r.chainId);
        });

    py::class_<Chain, std::shared_ptr<Chain>>(m, "Chain")
        .def(py::init())
        .def(py::init([](size_t id, const std::string& name) -> Chain {
                 return {id, name};
             }),
             py::arg("id"), py::arg("name"))
        .def_readwrite("id", &Chain::id)
        .def_readwrite("name", &Chain::name)
        .def("__repr__", [](Chain& c) { return fmt::format("<Chain: {}, '{}'>", c.id, c.name); });

    py::class_<MolecularData, std::shared_ptr<MolecularData>>(m, "MolecularData")
        .def(py::init())
        .def(py::init([](const std::string& source, Atoms atoms,
                         const std::vector<Residue>& residues = {},
                         const std::vector<Chain>& chains = {},
                         const std::vector<Bond>& bonds = {}) -> MolecularData {
                 return {source, std::move(atoms), residues, chains, bonds};
             }),
             py::arg("source"), py::arg("atoms"), py::arg("residues"), py::arg("chains"),
             py::arg("bonds"))
        .def_readwrite("source", &MolecularData::source)
        .def_readwrite("atoms", &MolecularData::atoms)
        .def_readwrite("residues", &MolecularData::residues)
        .def_readwrite("chains", &MolecularData::chains)
        .def_readwrite("bonds", &MolecularData::bonds)
        .def("__repr__", [](const MolecularData& md) {
            return fmt::format(
                "<MolecularStructure: '{}', {} atom(s), {} residue(s), {} chain(s), {} bonds>",
                md.source.value_or(""), md.atoms.positions.size(), md.residues.size(),
                md.chains.size(), md.bonds.size());
        });

    py::class_<MolecularStructure, std::shared_ptr<MolecularStructure>>(m, "MolecularStructure")
        .def(py::init([](MolecularData data) -> MolecularStructure { return {std::move(data)}; }),
             py::arg("data"))
        .def("hasResidues", &MolecularStructure::hasResidues)
        .def("hasResidue", &MolecularStructure::hasResidue, py::arg("residueId"),
             py::arg("chainId"))
        .def("hasChains", &MolecularStructure::hasChains)
        .def("hasChain", &MolecularStructure::hasChain, py::arg("chainId"))
        .def("getResidueAtoms", &MolecularStructure::getResidueAtoms, py::arg("residueId"),
             py::arg("chainId"))
        .def("getChainResidues", &MolecularStructure::getChainResidues, py::arg("chainId"))
        .def("getResidueIndices", &MolecularStructure::getResidueIndices)

        .def("__repr__", [](const MolecularStructure& s) {
            return fmt::format("<MolecularStructure: {} atom(s), residues {}, chains {}>",
                               s.get().atoms.positions.size(), s.hasResidues() ? "yes" : "no",
                               s.hasChains() ? "yes" : "no");
        });

    exposeStandardDataPorts<MolecularStructure>(m, "MolecularStructure");
}

}  // namespace inviwo
