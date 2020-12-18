/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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
#include <inviwo/molvisbase/util/utilities.h>

#include <inviwo/core/util/glmvec.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/molvisbase/util/utilities.h>

#include <string_view>
#include <array>
#include <algorithm>

namespace inviwo {

namespace molvis {

// clang-format off
enum class AminoAcid : unsigned char {
    Unknown = 0,
    Ala, Arg, Asn, Asp, Cys, Gln, Glu, Gly,
    His, Ile, Leu, Lys, Met, Phe, Pro, Ser,
    Thr, Trp, Tyr, Val, Sec, Pyl, Asx, Glx,
    Xle,
};
// clang-format on

namespace aminoacid {

enum class Colormap { Amino, Shapely, Ugene };

constexpr int num_aminoacids = 26;

namespace detail {

// clang-format off
constexpr std::array<std::string_view, num_aminoacids> names = {
    "Unknown",    "Alanine",       "Arginine",      "Asparagine",     "Aspartic acid", "Cysteine",
    "Glutamine",  "Glutamic acid", "Glycine",       "Histidine",      "Isoleucine",    "Leucine",
    "Lysine",     "Methionine",    "Phenylalanine", "Proline",        "Serine",        "Threonine",
    "Tryptophan", "Tyrosine",      "Valine",        "Selenocysteine", "Pyrrolysine",   
    "Asparagine or Aspartic acid", "Glutamine or Glutamic acid",  "Leucine or Isoleucine",
};

constexpr std::array<char, num_aminoacids> letter = {
    'X', 'A', 'R', 'N', 'D', 'C', 'Q', 'E', 'G', 'H', 'I', 'L', 'K', 
    'M', 'F', 'P', 'S', 'T', 'W', 'Y', 'V', 'U', 'O', 'B', 'Z', 'J',
};
// clang-format on

constexpr std::array<std::string_view, num_aminoacids> symbols = {
    "Xaa", "Ala", "Arg", "Asn", "Asp", "Cys", "Gln", "Glu", "Gly", "His", "Ile", "Leu", "Lys",
    "Met", "Phe", "Pro", "Ser", "Thr", "Trp", "Tyr", "Val", "Sec", "Pyl", "Asx", "Glx", "Xle",
};

// http://jmol.sourceforge.net/jscolors/ (stored as RGBA from least to most significant bit)
// Rasmol/Jmol colors http://acces.ens-lyon.fr/biotic/rastop/help/colour.htm#aminocolours
constexpr std::array<unsigned int, num_aminoacids> colorsAminoHex = {
    0xFFFF00FF, 0xFFC8C8C8, 0xFFFF5A14, 0xFFDCDC00, 0xFF0A0AE6, 0xFF00E6E6, 0xFFDCDC00,
    0xFF0A0AE6, 0xFFEBEBEB, 0xFFD28282, 0xFF0F820F, 0xFF0F820F, 0xFFFF5A14, 0xFF00E6E6,
    0xFFAA3232, 0xFF8296DC, 0xFF0096FA, 0xFF0096FA, 0xFFB45AB4, 0xFFAA3232, 0xFF0F820F,
    0xFF6EA0BE, 0xFF6EA0BE, 0xFFB469FF, 0xFFB469FF, 0xFF6EA0BE,
};

// Shapely http://acces.ens-lyon.fr/biotic/rastop/help/colour.htm#shapelycolours
// RCSB PDB https://www.rcsb.org/pages/help/3dview
constexpr std::array<unsigned int, num_aminoacids> colorsShapelyHex = {
    0xFFFF00FF, 0xFF8CFF8C, 0xFF7C0000, 0xFF707CFF, 0xFF4200A0, 0xFF70FFFF, 0xFF4C4CFF,
    0xFF000066, 0xFFFFFFFF, 0xFFFF7070, 0xFF004C00, 0xFF455E45, 0xFFB84747, 0xFF42A0B8,
    0xFF524C53, 0xFF525252, 0xFF4270FF, 0xFF004CB8, 0xFF00464F, 0xFF4C708C, 0xFFFF8CFF,
    0xFFC8C8C8, 0xFFC8C8C8, 0xFFC8C8C8, 0xFFC8C8C8, 0xFFC8C8C8,
};

// Ugene Multiple-Alignment http://ugene.net/forum/YaBB.pl?num=1337064665
constexpr std::array<unsigned int, num_aminoacids> colorsUgeneHex = {
    0xFFFF00FF, 0xFFC9FFFF, 0xFFFA9486, 0xFFA772EE, 0xFF7979FB, 0xFFADF9E3, 0xFFE3C3F9,
    0xFF3333F9, 0xFFC0C0C0, 0xFFFBF6D5, 0xFF4FFFFF, 0xFF79FFFF, 0xFFFCAABA, 0xFF27EDC3,
    0xFF8AC8C7, 0xFFF3F2F1, 0xFFC89ECA, 0xFFEFE4F0, 0xFFCDB085, 0xFFB9AF7D, 0xFFABFFFF,
    0xFFC8C8C8, 0xFFC8C8C8, 0xFFC8C8C8, 0xFFC8C8C8, 0xFFC8C8C8,
};

constexpr std::array<glm::vec4, num_aminoacids> colorsAmino = util::make_array<num_aminoacids>(
    [](auto index) { return molvisutil::colorFromHex(colorsAminoHex[index]); });

constexpr std::array<glm::vec4, num_aminoacids> colorsShapely = util::make_array<num_aminoacids>(
    [](auto index) { return molvisutil::colorFromHex(colorsShapelyHex[index]); });

constexpr std::array<glm::vec4, num_aminoacids> colorsUgene = util::make_array<num_aminoacids>(
    [](auto index) { return molvisutil::colorFromHex(colorsUgeneHex[index]); });

}  // namespace detail

// AminoAcid functions
constexpr AminoAcid aminoacid(int index) noexcept { return static_cast<AminoAcid>(index); }
constexpr int index(AminoAcid a) noexcept { return static_cast<int>(a); }
constexpr std::string_view name(AminoAcid a) noexcept { return detail::names[index(a)]; }
constexpr std::string_view letter(AminoAcid a) noexcept { return {&detail::letter[index(a)], 1}; }
constexpr std::string_view symbol(AminoAcid a) noexcept { return detail::symbols[index(a)]; }
constexpr vec4 color(AminoAcid a) noexcept { return detail::colorsAmino[index(a)]; }
constexpr vec4 color(AminoAcid a, Colormap map) noexcept {
    switch (map) {
        case Colormap::Amino:
            return detail::colorsAmino[index(a)];
        case Colormap::Shapely:
            return detail::colorsShapely[index(a)];
        case Colormap::Ugene:
            return detail::colorsUgene[index(a)];
        default:
            return color(a);
    }
}

constexpr AminoAcid fromLetter(std::string_view letter) noexcept {
    if (letter.size() != 1) return AminoAcid::Unknown;
    const char c = (letter[0] >= 'a' && letter[0] <= 'z') ? letter[0] - 'a' + 'A' : letter[0];
    auto it = std::find(detail::letter.begin(), detail::letter.end(), c);
    if (it == detail::letter.end()) return AminoAcid::Unknown;
    return aminoacid(static_cast<int>(it - detail::letter.begin()));
}

constexpr AminoAcid fromAbbr(std::string_view abbr) noexcept {
    if (abbr.size() != 3) return AminoAcid::Unknown;
    const std::array<char, 3> str = {
        (abbr[0] >= 'a' && abbr[0] <= 'z') ? abbr[0] - 'a' + 'A' : abbr[0],
        (abbr[1] >= 'A' && abbr[1] <= 'Z') ? abbr[1] - 'A' + 'a' : abbr[1],
        (abbr[2] >= 'A' && abbr[2] <= 'Z') ? abbr[2] - 'A' + 'a' : abbr[2]};
    auto it =
        std::find(detail::symbols.begin(), detail::symbols.end(), std::string_view(str.data(), 3));
    if (it == detail::symbols.end()) {
        return AminoAcid::Unknown;
    }
    return aminoacid(static_cast<int>(it - detail::symbols.begin()));
}

constexpr AminoAcid fromFullName(std::string_view fullName) noexcept {
    const auto trimmed = util::trim(fullName);
    if (auto elem = fromAbbr(trimmed.substr(0, 3)); elem != AminoAcid::Unknown) {
        return elem;
    }
    return fromLetter(trimmed.substr(0, 1));
}

}  // namespace aminoacid

}  // namespace molvis

}  // namespace inviwo
