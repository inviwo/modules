/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2025 Inviwo Foundation
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
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/molvisbase/util/utilities.h>

#include <string_view>
#include <array>
#include <optional>

namespace inviwo {

namespace molvis {

//
// element lookup tables and functions taken from mdutils by Robin Skånberg provided under the MIT
// license https://github.com/scanberg/mdutils
//

// clang-format off
enum class Element : unsigned char {
    Unknown = 0,
    H, He, Li, Be, B, C, N, O, F, Ne, Na, Mg, Al, Si, P, S, Cl, Ar, K, Ca, Sc, Ti, V,
    Cr, Mn, Fe, Co, Ni, Cu, Zn, Ga, Ge, As, Se, Br, Kr, Rb, Sr, Y, Zr, Nb, Mo, Tc, Ru,
    Rh, Pd, Ag, Cd, In, Sn, Sb, Te, I, Xe, Cs, Ba, La, Ce, Pr, Nd, Pm, Sm, Eu, Gd, Tb,
    Dy, Ho, Er, Tm, Yb, Lu, Hf, Ta, W, Re, Os, Ir, Pt, Au, Hg, Tl, Pb, Bi, Po, At, Rn,
    Fr, Ra, Ac, Th, Pa, U, Np, Pu, Am, Cm, Bk, Cf, Es, Fm, Md, No, Lr, Rf, Db, Sg, Bh,
    Hs, Mt, Ds, Rg, Cn, Nh, Fl, Mc, Lv, Ts, Og
};
// clang-format on

namespace element {

enum class Colormap { RasmolCPK, RasmolCPKnew, Jmol };

constexpr int num_elements = 119;

namespace detail {

constexpr std::array<std::string_view, num_elements> names = {
    "Unknown",    "Hydrogen",     "Helium",        "Lithium",     "Beryllium",   "Boron",
    "Carbon",     "Nitrogen",     "Oxygen",        "Fluorine",    "Neon",        "Sodium",
    "Magnesium",  "Aluminium",    "Silicon",       "Phosphorus",  "Sulfur",      "Chlorine",
    "Argon",      "Potassium",    "Calcium",       "Scandium",    "Titanium",    "Vanadium",
    "Chromium",   "Manganese",    "Iron",          "Cobalt",      "Nickel",      "Copper",
    "Zinc",       "Gallium",      "Germanium",     "Arsenic",     "Selenium",    "Bromine",
    "Krypton",    "Rubidium",     "Strontium",     "Yttrium",     "Zirconium",   "Niobium",
    "Molybdenum", "Technetium",   "Ruthenium",     "Rhodium",     "Palladium",   "Silver",
    "Cadmium",    "Indium",       "Tin",           "Antimony",    "Tellurium",   "Iodine",
    "Xenon",      "Caesium",      "Barium",        "Lanthanum",   "Cerium",      "Praseodymium",
    "Neodymium",  "Promethium",   "Samarium",      "Europium",    "Gadolinium",  "Terbium",
    "Dysprosium", "Holmium",      "Erbium",        "Thulium",     "Ytterbium",   "Lutetium",
    "Hafnium",    "Tantalum",     "Tungsten",      "Rhenium",     "Osmium",      "Iridium",
    "Platinum",   "Gold",         "Mercury",       "Thallium",    "Lead",        "Bismuth",
    "Polonium",   "Astatine",     "Radon",         "Francium",    "Radium",      "Actinium",
    "Thorium",    "Protactinium", "Uranium",       "Neptunium",   "Plutonium",   "Americium",
    "Curium",     "Berkelium",    "Californium",   "Einsteinium", "Fermium",     "Mendelevium",
    "Nobelium",   "Lawrencium",   "Rutherfordium", "Dubnium",     "Seaborgium",  "Bohrium",
    "Hassium",    "Meitnerium",   "Darmstadtium",  "Roentgenium", "Copernicium", "Nihonium",
    "Flerovium",  "Moscovium",    "Livermorium",   "Tennessine",  "Oganesson",
};

constexpr std::array<std::string_view, num_elements> symbols = {
    "Xx", "H",  "He", "Li", "Be", "B",  "C",  "N",  "O",  "F",  "Ne", "Na", "Mg", "Al", "Si",
    "P",  "S",  "Cl", "Ar", "K",  "Ca", "Sc", "Ti", "V",  "Cr", "Mn", "Fe", "Co", "Ni", "Cu",
    "Zn", "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y",  "Zr", "Nb", "Mo", "Tc", "Ru",
    "Rh", "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te", "I",  "Xe", "Cs", "Ba", "La", "Ce", "Pr",
    "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf", "Ta", "W",
    "Re", "Os", "Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac",
    "Th", "Pa", "U",  "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr", "Rf",
    "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Cn", "Nh", "Fl", "Mc", "Lv", "Ts", "Og",
};

// http://dx.doi.org/10.1039/b801115j
constexpr std::array<double, num_elements> covalent_radii = {
    0.00, 0.31, 0.28, 1.28, 0.96, 0.84, 0.76, 0.71, 0.66, 0.57, 0.58, 1.66, 1.41, 1.21, 1.11,
    1.07, 1.05, 1.02, 1.06, 2.03, 1.76, 1.70, 1.60, 1.53, 1.39, 1.39, 1.32, 1.26, 1.24, 1.32,
    1.22, 1.22, 1.20, 1.19, 1.20, 1.20, 1.16, 2.20, 1.95, 1.90, 1.75, 1.64, 1.54, 1.47, 1.46,
    1.42, 1.39, 1.45, 1.44, 1.42, 1.39, 1.39, 1.38, 1.39, 1.40, 2.44, 2.15, 2.07, 2.04, 2.03,
    2.01, 1.99, 1.98, 1.98, 1.96, 1.94, 1.92, 1.92, 1.89, 1.90, 1.87, 1.87, 1.75, 1.70, 1.62,
    1.51, 1.44, 1.41, 1.36, 1.36, 1.32, 1.45, 1.46, 1.48, 1.40, 1.50, 1.50, 2.60, 2.21, 2.15,
    2.06, 2.00, 1.96, 1.90, 1.87, 1.80, 1.69, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
    1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
};

// https://dx.doi.org/10.1021/jp8111556
constexpr std::array<double, num_elements> vdw_radii = {
    1.00, 1.10, 1.40, 1.81, 1.53, 1.92, 1.70, 1.55, 1.52, 1.47, 1.54, 2.27, 1.73, 1.84, 2.10,
    1.80, 1.80, 1.75, 1.88, 2.75, 2.31, 2.30, 2.15, 2.05, 2.05, 2.05, 2.05, 2.00, 2.00, 2.00,
    2.10, 1.87, 2.11, 1.85, 1.90, 1.83, 2.02, 3.03, 2.49, 2.40, 2.30, 2.15, 2.10, 2.05, 2.05,
    2.00, 2.05, 2.10, 2.20, 2.20, 1.93, 2.17, 2.06, 1.98, 2.16, 3.43, 2.68, 2.50, 2.48, 2.47,
    2.45, 2.43, 2.42, 2.40, 2.38, 2.37, 2.35, 2.33, 2.32, 2.30, 2.28, 2.27, 2.25, 2.20, 2.10,
    2.05, 2.00, 2.00, 2.05, 2.10, 2.05, 1.96, 2.02, 2.07, 1.97, 2.02, 2.20, 3.48, 2.83, 2.00,
    2.40, 2.00, 2.30, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00,
    2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00,
};

// http://chemistry.wikia.com/wiki/List_of_elements_by_atomic_mass
// last element padded with 295
constexpr std::array<double, num_elements> atomic_mass = {
    0,       1.00794,    4.002602,  6.941,       9.012182, 10.811,     12.0107, 14.0067,
    15.9994, 18.9984032, 20.1797,   22.98976928, 24.3050,  26.9815386, 28.0855, 30.973762,
    32.065,  35.453,     39.0983,   39.948,      40.078,   44.955912,  47.867,  50.9415,
    51.9961, 54.938045,  55.845,    58.933195,   58.6934,  63.546,     65.409,  69.723,
    72.64,   74.92160,   78.96,     79.904,      83.798,   85.4678,    87.62,   88.90585,
    91.224,  92.906,     95.94,     98,          101.07,   102.905,    106.42,  107.8682,
    112.411, 114.818,    118.710,   121.760,     127.60,   126.904,    131.293, 132.9054519,
    137.327, 138.90547,  140.116,   140.90765,   144.242,  145,        150.36,  151.964,
    157.25,  158.92535,  162.500,   164.930,     167.259,  168.93421,  173.04,  174.967,
    178.49,  180.94788,  183.84,    186.207,     190.23,   192.217,    195.084, 196.966569,
    200.59,  204.3833,   207.2,     208.98040,   210,      210,        220,     223,
    226,     227,        231.03588, 232.03806,   237,      238.02891,  243,     244,
    247,     247,        251,       252,         257,      258,        259,     262,
    261,     262,        266,       264,         277,      268,        271,     272,
    285,     284,        289,       288,         292,      294,        295,
};

// http://jmol.sourceforge.net/jscolors/ (stored as RGBA from least to most significant bit)
// Rasmol colors
constexpr std::array<unsigned int, num_elements> colorsRasmolHex = {
    0xFFFF00FF, 0xFFFFFFFF, 0xFFFFFFD9, 0xFF2222B2, 0xFF00FFC2, 0xFF00FF00, 0xFFC8C8C8, 0xFFFF8F8F,
    0xFF0000F0, 0xFF50E090, 0xFFF5E3B3, 0xFFF25CAB, 0xFF00FF8A, 0xFF908080, 0xFFA0C8F0, 0xFF00A5FF,
    0xFF32C8FF, 0xFF1FF01F, 0xFFE3D180, 0xFFD4408F, 0xFF908080, 0xFFE6E6E6, 0xFF908080, 0xFFABA6A6,
    0xFF908080, 0xFF908080, 0xFF00A5FF, 0xFFA090F0, 0xFF2A2AA5, 0xFF2A2AA5, 0xFF2A2AA5, 0xFF8F8FC2,
    0xFF8F8F66, 0xFFE380BD, 0xFF00A1FF, 0xFF2A2AA5, 0xFFD1B85C, 0xFFB02E70, 0xFF00FF00, 0xFFFFFF94,
    0xFFE0E094, 0xFFC9C273, 0xFFB5B554, 0xFF9E9E3B, 0xFF8F8F24, 0xFF8C7D0A, 0xFF856900, 0xFF908080,
    0xFF8FD9FF, 0xFF7375A6, 0xFF808066, 0xFFB5639E, 0xFF007AD4, 0xFF940094, 0xFFB09E42, 0xFF8F1757,
    0xFF00A5FF, 0xFFFFD470, 0xFFC7FFFF, 0xFFC7FFD9, 0xFFC7FFC7, 0xFFC7FFA3, 0xFFC7FF8F, 0xFFC7FF61,
    0xFFC7FF45, 0xFFC7FF30, 0xFFC7FF1F, 0xFF9CFF00, 0xFF75E600, 0xFF52D400, 0xFF38BF00, 0xFF24AB00,
    0xFFFFC24D, 0xFFFFA64D, 0xFFD69421, 0xFFAB7D26, 0xFF966626, 0xFF875417, 0xFFE0D0D0, 0xFF23D1FF,
    0xFFD0B8B8, 0xFF4D54A6, 0xFF615957, 0xFFB54F9E, 0xFF005CAB, 0xFF454F75, 0xFF968242, 0xFF660042,
    0xFF007D00, 0xFFFAAB70, 0xFFFFBA00, 0xFFFFA100, 0xFFFF8F00, 0xFFFF8000, 0xFFFF6B00, 0xFFF25C54,
    0xFFE35C78, 0xFFE34F8A, 0xFFD436A1, 0xFFD41FB3, 0xFFBA1FB3, 0xFFA60DB3, 0xFF870DBD, 0xFF6600C7,
    0xFF5900CC, 0xFF4F00D1, 0xFF4500D9, 0xFF3800E0, 0xFF2E00E6, 0xFF2600EB, 0xFF2200F0, 0xFF2000F6,
    0xFF1E00F8, 0xFF1C00FA, 0xFF1A00FC, 0xFF1800FD, 0xFF1600FE, 0xFF1400FF, 0xFF1200FF,
};

// Rasmol CPKnew colors
constexpr std::array<unsigned int, num_elements> colorsRasmolCPKnewHex = {
    0xFFFF00FF, 0xFFFFFFFF, 0xFFFFFFD9, 0xFF2121B2, 0xFF00FFC2, 0xFF00FF00, 0xFFD3D3D3, 0xFFE6CE87,
    0xFF0000FF, 0xFF50E090, 0xFFF5E3B3, 0xFFF25CAB, 0xFF00FF8A, 0xFF696969, 0xFFA0C8F0, 0xFF00AAFF,
    0xFF00FFFF, 0xFF1FF01F, 0xFFE3D180, 0xFFD4408F, 0xFF696969, 0xFFE6E6E6, 0xFF696969, 0xFFABA6A6,
    0xFF696969, 0xFF696969, 0xFF00AAFF, 0xFFA090F0, 0xFF282880, 0xFF282880, 0xFF282880, 0xFF8F8FC2,
    0xFF8F8F66, 0xFFE380BD, 0xFF00A1FF, 0xFF282880, 0xFFD1B85C, 0xFFB02E70, 0xFF00FF00, 0xFFFFFF94,
    0xFFE0E094, 0xFFC9C273, 0xFFB5B554, 0xFF9E9E3B, 0xFF8F8F24, 0xFF8C7D0A, 0xFF856900, 0xFF696969,
    0xFF8FD9FF, 0xFF7375A6, 0xFF808066, 0xFFB5639E, 0xFF007AD4, 0xFF940094, 0xFFB09E42, 0xFF8F1757,
    0xFF00AAFF, 0xFFFFD470, 0xFFC7FFFF, 0xFFC7FFD9, 0xFFC7FFC7, 0xFFC7FFA3, 0xFFC7FF8F, 0xFFC7FF61,
    0xFFC7FF45, 0xFFC7FF30, 0xFFC7FF1F, 0xFF9CFF00, 0xFF75E600, 0xFF52D400, 0xFF38BF00, 0xFF24AB00,
    0xFFFFC24D, 0xFFFFA64D, 0xFFD69421, 0xFFAB7D26, 0xFF966626, 0xFF875417, 0xFFE0D0D0, 0xFF23D1FF,
    0xFFD0B8B8, 0xFF4D54A6, 0xFF615957, 0xFFB54F9E, 0xFF005CAB, 0xFF454F75, 0xFF968242, 0xFF660042,
    0xFF007D00, 0xFFFAAB70, 0xFFFFBA00, 0xFFFFA100, 0xFFFF8F00, 0xFFFF8000, 0xFFFF6B00, 0xFFF25C54,
    0xFFE35C78, 0xFFE34F8A, 0xFFD436A1, 0xFFD41FB3, 0xFFBA1FB3, 0xFFA60DB3, 0xFF870DBD, 0xFF6600C7,
    0xFF5900CC, 0xFF4F00D1, 0xFF4500D9, 0xFF3800E0, 0xFF2E00E6, 0xFF2600EB, 0xFF2200F0, 0xFF2000F6,
    0xFF1E00F8, 0xFF1C00FA, 0xFF1A00FC, 0xFF1800FD, 0xFF1600FE, 0xFF1400FF, 0xFF1200FF,
};

constexpr std::array<unsigned int, num_elements> colorsJmolHex = {
    0xFFFF00FF, 0xFFFFFFFF, 0xFFFFFFD9, 0xFFFF80CC, 0xFF00FFC2, 0xFFB5B5FF, 0xFF909090, 0xFFF85030,
    0xFF0D0DFF, 0xFF50E090, 0xFFF5E3B3, 0xFFF25CAB, 0xFF00FF8A, 0xFFA6A6BF, 0xFFA0C8F0, 0xFF0080FF,
    0xFF30FFFF, 0xFF1FF01F, 0xFFE3D180, 0xFFD4408F, 0xFF00FF3D, 0xFFE6E6E6, 0xFFC7C2BF, 0xFFABA6A6,
    0xFFC7998A, 0xFFC77A9C, 0xFF3366E0, 0xFFA090F0, 0xFF50D050, 0xFF3380C8, 0xFFB0807D, 0xFF8F8FC2,
    0xFF8F8F66, 0xFFE380BD, 0xFF00A1FF, 0xFF2929A6, 0xFFD1B85C, 0xFFB02E70, 0xFF00FF00, 0xFFFFFF94,
    0xFFE0E094, 0xFFC9C273, 0xFFB5B554, 0xFF9E9E3B, 0xFF8F8F24, 0xFF8C7D0A, 0xFF856900, 0xFFC0C0C0,
    0xFF8FD9FF, 0xFF7375A6, 0xFF808066, 0xFFB5639E, 0xFF007AD4, 0xFF940094, 0xFFB09E42, 0xFF8F1757,
    0xFF00C900, 0xFFFFD470, 0xFFC7FFFF, 0xFFC7FFD9, 0xFFC7FFC7, 0xFFC7FFA3, 0xFFC7FF8F, 0xFFC7FF61,
    0xFFC7FF45, 0xFFC7FF30, 0xFFC7FF1F, 0xFF9CFF00, 0xFF75E600, 0xFF52D400, 0xFF38BF00, 0xFF24AB00,
    0xFFFFC24D, 0xFFFFA64D, 0xFFD69421, 0xFFAB7D26, 0xFF966626, 0xFF875417, 0xFFE0D0D0, 0xFF23D1FF,
    0xFFD0B8B8, 0xFF4D54A6, 0xFF615957, 0xFFB54F9E, 0xFF005CAB, 0xFF454F75, 0xFF968242, 0xFF660042,
    0xFF007D00, 0xFFFAAB70, 0xFFFFBA00, 0xFFFFA100, 0xFFFF8F00, 0xFFFF8000, 0xFFFF6B00, 0xFFF25C54,
    0xFFE35C78, 0xFFE34F8A, 0xFFD436A1, 0xFFD41FB3, 0xFFBA1FB3, 0xFFA60DB3, 0xFF870DBD, 0xFF6600C7,
    0xFF5900CC, 0xFF4F00D1, 0xFF4500D9, 0xFF3800E0, 0xFF2E00E6, 0xFF2600EB, 0xFF2200F0, 0xFF2000F6,
    0xFF1E00F8, 0xFF1C00FA, 0xFF1A00FC, 0xFF1800FD, 0xFF1600FE, 0xFF1400FF, 0xFF1200FF,
};

constexpr std::array<glm::vec4, num_elements> colorsRasmol = util::make_array<num_elements>(
    [](auto index) { return molvisutil::colorFromHex(colorsRasmolHex[index]); });

constexpr std::array<glm::vec4, num_elements> colorsRasmolCPKnew = util::make_array<num_elements>(
    [](auto index) { return molvisutil::colorFromHex(colorsRasmolCPKnewHex[index]); });

constexpr std::array<glm::vec4, num_elements> colorsJmol = util::make_array<num_elements>(
    [](auto index) { return molvisutil::colorFromHex(colorsJmolHex[index]); });

constexpr std::array<Element, 17> nonmetallic = {
    Element::H,  Element::He, Element::C, Element::N,  Element::O,  Element::F,
    Element::Ne, Element::P,  Element::S, Element::Cl, Element::Ar, Element::Se,
    Element::Br, Element::Kr, Element::I, Element::Xe, Element::Rn,
};

}  // namespace detail

// Element functions
constexpr Element element(int atomicNumber) noexcept { return static_cast<Element>(atomicNumber); }
constexpr int atomicNumber(Element symbol) noexcept { return static_cast<int>(symbol); }
constexpr std::string_view name(Element symbol) noexcept {
    return detail::names[atomicNumber(symbol)];
}
constexpr std::string_view symbol(Element symbol) noexcept {
    return detail::symbols[atomicNumber(symbol)];
}
constexpr vec4 color(Element symbol) noexcept {
    return detail::colorsRasmolCPKnew[atomicNumber(symbol)];
}
constexpr vec4 color(Element symbol, Colormap map) noexcept {
    switch (map) {
        case Colormap::RasmolCPK:
            return detail::colorsRasmol[atomicNumber(symbol)];
        case Colormap::RasmolCPKnew:
            return detail::colorsRasmolCPKnew[atomicNumber(symbol)];
        case Colormap::Jmol:
            return detail::colorsJmol[atomicNumber(symbol)];
        default:
            return color(symbol);
    }
}
constexpr double vdwRadius(Element symbol) noexcept {
    return detail::vdw_radii[atomicNumber(symbol)];
}
constexpr double covalentRadius(Element symbol) noexcept {
    return detail::covalent_radii[atomicNumber(symbol)];
}
constexpr double atomicMass(Element symbol) noexcept {
    return detail::atomic_mass[atomicNumber(symbol)];
}
constexpr bool isNonMetallic(Element symbol) noexcept {
    return std::find(detail::nonmetallic.begin(), detail::nonmetallic.end(), symbol) !=
           detail::nonmetallic.end();
}
constexpr bool isMetallic(Element symbol) noexcept { return !isNonMetallic(symbol); }

constexpr Element fromAbbr(std::string_view abbr) noexcept {
    // create a (linear) hash function for all combinations of single letter and two letter atomic
    // elements
    constexpr auto index = [](std::string_view abbr) {
        if (abbr.size() > 2) return 0;
        int index = 1;
        if (abbr.size() > 0) {
            const char c0 = (abbr[0] >= 'a' && abbr[0] <= 'z') ? abbr[0] - 'a' + 'A' : abbr[0];
            if (c0 < 'A' || c0 > 'Z') return 0;
            index += (c0 - 'A') * ('z' - 'a' + 1);
        }
        if (abbr.size() > 1) {
            const int c1 = (abbr[1] >= 'A' && abbr[1] <= 'Z') ? abbr[1] - 'A' + 'a' : abbr[1];
            if (c1 < 'a' || c1 > 'z') return 0;
            index += c1 - 'a' + 1;
        }
        return index;
    };

    // create a lookup table for all existing atomic elements, std::nullopt otherwise
    constexpr auto lookup = [=]() {
        constexpr int size = 1 + ('Z' - 'A') * ('z' - 'a' + 2);
        std::array<Element, size> table = {Element::Unknown};
        for (size_t i = 0; i < detail::symbols.size(); ++i) {
            const auto ind = index(detail::symbols[i]);
            table[ind] = element(static_cast<int>(i));
        }
        return table;
    }();

    return lookup[index(abbr)];
}

constexpr Element fromFullName(std::string_view fullAtomName) noexcept {
    const auto trimmed = util::trim(fullAtomName);
    if (auto elem = fromAbbr(trimmed.substr(0, 2)); elem != Element::Unknown) {
        return elem;
    }
    return fromAbbr(trimmed.substr(0, 1));
}

}  // namespace element

}  // namespace molvis

}  // namespace inviwo
