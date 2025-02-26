/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2025 Inviwo Foundation
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
#include <inviwo/molvisbase/util/aminoacid.h>
#include <inviwo/molvisbase/util/chain.h>
#include <modules/python3/pyportutils.h>
#include <modules/python3/opaquetypes.h>
#include <modules/python3/polymorphictypehooks.h>

#include <fmt/format.h>

namespace py = pybind11;

namespace inviwo {

using namespace molvis;

namespace detail {

void exposeAtomicElement(pybind11::module& m) {
    m.def("element", element::element, py::arg("atomicNumber"))
        .def("atomicNumber", element::atomicNumber, py::arg("symbol"))
        .def("name", element::name, py::arg("symbol"))
        .def("symbol", element::symbol, py::arg("symbol"))
        .def("color", py::overload_cast<Element>(element::color), py::arg("symbol"))
        .def("color", py::overload_cast<Element, element::Colormap>(element::color),
             py::arg("symbol"), py::arg("colormap"))
        .def("vdwRadius", element::vdwRadius, py::arg("symbol"))
        .def("covalentRadius", element::covalentRadius, py::arg("symbol"))
        .def("atomicMass", element::atomicMass, py::arg("symbol"))
        .def("fromAbbr", element::fromAbbr, py::arg("abbr"))
        .def("fromFullName", element::fromFullName, py::arg("fullName"));

    auto colors = m.def_submodule("colors", "Various color lookup tables");
    colors.attr("rasmol") = py::cast(element::detail::colorsRasmol);
    colors.attr("rasmolCPKnew") = py::cast(element::detail::colorsRasmolCPKnew);
    colors.attr("jmol") = py::cast(element::detail::colorsJmol);

    py::enum_<element::Colormap>(m, "Colormap")
        .value("RasmolCPK", element::Colormap::RasmolCPK)
        .value("RasmolCPKnew", element::Colormap::RasmolCPKnew)
        .value("Jmol", element::Colormap::Jmol);

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

void exposeAminoAcid(pybind11::module& m) {
    m.def("aminoacid", aminoacid::aminoacid, py::arg("index"))
        .def("index", aminoacid::index, py::arg("aminoacid"))
        .def("name", aminoacid::name, py::arg("aminoacid"))
        .def("letter", aminoacid::letter, py::arg("aminoacid"))
        .def("symbol", aminoacid::symbol, py::arg("aminoacid"))
        .def("color", py::overload_cast<AminoAcid>(aminoacid::color), py::arg("aminoacid"))
        .def("color", py::overload_cast<AminoAcid, aminoacid::Colormap>(aminoacid::color),
             py::arg("aminoacid"), py::arg("colormap"))
        .def("fromLetter", aminoacid::fromLetter, py::arg("letter"))
        .def("fromAbbr", aminoacid::fromAbbr, py::arg("abbr"))
        .def("fromFullName", aminoacid::fromFullName, py::arg("fullName"));

    auto colors = m.def_submodule("colors", "Various color lookup tables");
    colors.attr("amino") = py::cast(aminoacid::detail::colorsAmino);
    colors.attr("shapely") = py::cast(aminoacid::detail::colorsShapely);
    colors.attr("ugene") = py::cast(aminoacid::detail::colorsUgene);

    py::enum_<aminoacid::Colormap>(m, "Colormap")
        .value("Amino", aminoacid::Colormap::Amino)
        .value("Shapely", aminoacid::Colormap::Shapely)
        .value("Ugene", aminoacid::Colormap::Ugene);

    py::enum_<AminoAcid>(m, "AminoAcid")
        .value("Unknown", AminoAcid::Unknown)
        .value("Ala", AminoAcid::Ala)
        .value("Arg", AminoAcid::Arg)
        .value("Asn", AminoAcid::Asn)
        .value("Asp", AminoAcid::Asp)
        .value("Cys", AminoAcid::Cys)
        .value("Gln", AminoAcid::Gln)
        .value("Glu", AminoAcid::Glu)
        .value("Gly", AminoAcid::Gly)
        .value("His", AminoAcid::His)
        .value("Ile", AminoAcid::Ile)
        .value("Leu", AminoAcid::Leu)
        .value("Lys", AminoAcid::Lys)
        .value("Met", AminoAcid::Met)
        .value("Phe", AminoAcid::Phe)
        .value("Pro", AminoAcid::Pro)
        .value("Ser", AminoAcid::Ser)
        .value("Thr", AminoAcid::Thr)
        .value("Trp", AminoAcid::Trp)
        .value("Tyr", AminoAcid::Tyr)
        .value("Val", AminoAcid::Val)
        .value("Sec", AminoAcid::Sec)
        .value("Pyl", AminoAcid::Pyl)
        .value("Asx", AminoAcid::Asx)
        .value("Glx", AminoAcid::Glx)
        .value("Xle", AminoAcid::Xle);
}

void exposeChainId(pybind11::module& m) {
    m.def("chain", chain::chain, py::arg("index"))
        .def("index", chain::index, py::arg("chain"))
        .def("name", chain::name, py::arg("chain"))
        .def("fromId", chain::fromId, py::arg("id"))
        .def("fromName", chain::fromName, py::arg("name"))
        .def("fromFullName", chain::fromFullName, py::arg("fullName"))
        .def("color", py::overload_cast<ChainId>(chain::color), py::arg("chain"))
        .def("color", py::overload_cast<ChainId, chain::Colormap>(chain::color), py::arg("chain"),
             py::arg("colormap"))
        .def("color", py::overload_cast<int>(chain::color), py::arg("chainId"))
        .def("color", py::overload_cast<int, chain::Colormap>(chain::color), py::arg("chainId"),
             py::arg("colormap"))
        .def("color", py::overload_cast<std::string_view>(chain::color), py::arg("fullName"))
        .def("color", py::overload_cast<std::string_view, chain::Colormap>(chain::color),
             py::arg("fullName"), py::arg("colormap"));

    auto colors = m.def_submodule("colors", "Various color lookup tables");
    colors.attr("jmol") = py::cast(chain::detail::colorsJmol);
    colors.attr("jmolhetero") = py::cast(chain::detail::colorsJmolHetero);

    py::enum_<chain::Colormap>(m, "Colormap")
        .value("Jmol", chain::Colormap::Jmol)
        .value("JmolHetero", chain::Colormap::JmolHetero);

    py::enum_<ChainId>(m, "ChainId")
        .value("Unknown", ChainId::Unknown)
        .value("A", ChainId::A)
        .value("B", ChainId::B)
        .value("C", ChainId::C)
        .value("D", ChainId::D)
        .value("E", ChainId::E)
        .value("F", ChainId::F)
        .value("G", ChainId::G)
        .value("H", ChainId::H)
        .value("I", ChainId::I)
        .value("J", ChainId::J)
        .value("K", ChainId::K)
        .value("L", ChainId::L)
        .value("M", ChainId::M)
        .value("N", ChainId::N)
        .value("O", ChainId::O)
        .value("P", ChainId::P)
        .value("Q", ChainId::Q)
        .value("R", ChainId::R)
        .value("S", ChainId::S)
        .value("T", ChainId::T)
        .value("U", ChainId::U)
        .value("V", ChainId::V)
        .value("W", ChainId::W)
        .value("X", ChainId::X)
        .value("Y", ChainId::Y)
        .value("Z", ChainId::Z);
}

void exposeMolVisUtil(pybind11::module& m) {
    m.def("findResidue", &findResidue, py::arg("data"), py::arg("residueId"), py::arg("chainId"))
        .def("findChainId", &findChain, py::arg("data"), py::arg("chainId"))
        .def("getGlobalAtomIndex", &getGlobalAtomIndex, py::arg("atoms"), py::arg("fullAtomName"),
             py::arg("residueId"), py::arg("chainId"))
        .def("computeCovalentBonds", &computeCovalentBonds, py::arg("atoms"))
        .def("getAtomicNumbers", &getAtomicNumbers, py::arg("fullNames"))
        .def("createMesh", &createMesh, py::arg("structure"), py::arg("enablePicking") = false,
             py::arg("globalStartId") = 0);
}

}  // namespace detail

void exposeMolVis(pybind11::module& m) {
    auto mAtomicElement =
        m.def_submodule("atomicelement", "Atomic element lookup tables and functions for MolVis");
    detail::exposeAtomicElement(mAtomicElement);
    auto mAminoAcid =
        m.def_submodule("aminoacid", "Amino acid lookup tables and functions for MolVis");
    detail::exposeAminoAcid(mAminoAcid);
    auto mChainId = m.def_submodule("chainid", "Chain id lookup tables and functions for MolVis");
    detail::exposeChainId(mChainId);
    auto mUtil = m.def_submodule("util", "MolVis utils");
    detail::exposeMolVisUtil(mUtil);

    py::classh<Atoms>(m, "Atoms")
        .def(py::init())
        .def_readwrite("positions", &Atoms::positions)
        .def_readwrite("serialnumbers", &Atoms::serialNumbers)
        .def_readwrite("bfactors", &Atoms::bFactors)
        .def_readwrite("modelids", &Atoms::modelIds)
        .def_readwrite("chainids", &Atoms::chainIds)
        .def_readwrite("residueids", &Atoms::residueIds)
        .def_readwrite("atomicnumbers", &Atoms::atomicNumbers)
        .def_readwrite("fullnames", &Atoms::fullNames)
        .def("__len__", [](Atoms& a) { return a.positions.size(); })
        .def("__repr__", [](Atoms& a) {
            return fmt::format(
                "<Atoms: {} positions, {} serial numbers, {} b factors, {} model IDs, {} chain "
                "IDs, {} residue IDs, "
                "{} atomic numbers, {} full names>",
                a.positions.size(), a.serialNumbers.size(), a.bFactors.size(), a.modelIds.size(),
                a.chainIds.size(), a.residueIds.size(), a.atomicNumbers.size(), a.fullNames.size());
        });

    py::classh<Residue>(m, "Residue")
        .def(py::init())
        .def(py::init([](int id, AminoAcid aminoacid, const std::string& fullname,
                         int chainid) -> Residue {
                 return {id, aminoacid, fullname, chainid};
             }),
             py::arg("id"), py::arg("aminoacid"), py::arg("fullname"), py::arg("chainid"))
        .def(py::init([](int id, const std::string& name, const std::string& fullname,
                         int chainid) -> Residue {
                 return {id, aminoacid::fromAbbr(name), fullname, chainid};
             }),
             py::arg("id"), py::arg("name"), py::arg("fullname"), py::arg("chainid"))
        .def_readwrite("id", &Residue::id)
        .def_readwrite("aminoacid", &Residue::aminoacid)
        .def_readwrite("fullname", &Residue::fullName)
        .def_readwrite("chainid", &Residue::chainId)
        .def("__repr__", [](Residue& r) {
            return fmt::format("<Residue: {}, '{}' ({}), '{}', {}>", r.id,
                               aminoacid::symbol(r.aminoacid), aminoacid::letter(r.aminoacid),
                               r.fullName, r.chainId);
        });

    py::classh<Chain>(m, "Chain")
        .def(py::init())
        .def(py::init([](int id, const std::string& name) -> Chain {
                 return {id, name};
             }),
             py::arg("id"), py::arg("name"))
        .def_readwrite("id", &Chain::id)
        .def_readwrite("name", &Chain::name)
        .def("__repr__", [](Chain& c) { return fmt::format("<Chain: {}, '{}'>", c.id, c.name); });

    py::classh<MolecularData>(m, "MolecularData")
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

    py::classh<MolecularStructure>(m, "MolecularStructure")
        .def(py::init([](MolecularData data) -> MolecularStructure { return {std::move(data)}; }),
             py::arg("data"))
        .def_readwrite("axes", &MolecularStructure::axes)
        .def("data", &MolecularStructure::data)
        .def("atoms", &MolecularStructure::atoms)
        .def("residues", &MolecularStructure::residues)
        .def("chains", &MolecularStructure::chains)
        .def("bonds", &MolecularStructure::bonds)
        .def("hasResidues", &MolecularStructure::hasResidues)
        .def("hasResidue", &MolecularStructure::hasResidue, py::arg("residueId"),
             py::arg("chainId"))
        .def("hasChains", &MolecularStructure::hasChains)
        .def("hasChain", &MolecularStructure::hasChain, py::arg("chainId"))
        .def("getResidueAtoms", &MolecularStructure::getResidueAtoms, py::arg("residueId"),
             py::arg("chainId"))
        .def("getChainResidues", &MolecularStructure::getChainResidues, py::arg("chainId"))
        .def("getResidueIndices", &MolecularStructure::getResidueIndices)

        .def_property("modelMatrix", &MolecularStructure::getModelMatrix,
                      &MolecularStructure::setModelMatrix)
        .def_property("worldMatrix", &MolecularStructure::getWorldMatrix,
                      &MolecularStructure::setWorldMatrix)
        .def_property("basis", &MolecularStructure::getBasis, &MolecularStructure::setBasis)
        .def_property("offset", &MolecularStructure::getOffset, &MolecularStructure::setOffset)

        .def("__repr__", [](const MolecularStructure& s) {
            return fmt::format("<MolecularStructure: {} atom(s), residues {}, chains {}>",
                               s.atoms().positions.size(), s.hasResidues() ? "yes" : "no",
                               s.hasChains() ? "yes" : "no");
        });

    exposeStandardDataPorts<MolecularStructure>(m, "MolecularStructure");
}

}  // namespace inviwo
