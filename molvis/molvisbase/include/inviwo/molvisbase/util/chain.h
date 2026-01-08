/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2026 Inviwo Foundation
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
enum class ChainId : unsigned char {
    Unknown = 0,
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, 
};
// clang-format on

namespace chain {

enum class Colormap { Jmol, JmolHetero };

constexpr int num_chains = 27;

namespace detail {

// http://jmol.sourceforge.net/jscolors/ (stored as RGBA from least to most significant bit)
constexpr std::array<unsigned int, num_chains> colorsJmolHex = {
    0xFFC8C8C8, 0xFFFFD0C0, 0xFFB0FFB0, 0xFFC8C0FF, 0xFF80FFFF, 0xFFFFC0FF, 0xFFF0F0B0,
    0xFF70D0FF, 0xFF8080F0, 0xFFB3DEF5, 0xFFFFBF00, 0xFF5C5CCD, 0xFFAACD66, 0xFF32CD9A,
    0xFFEE82EE, 0xFFD1CE00, 0xFF7FFF00, 0xFF71B33C, 0xFF8B0000, 0xFF6BB7BD, 0xFF006400,
    0xFF000080, 0xFF008080, 0xFF800080, 0xFF808000, 0xFF0B86B8, 0xFF2222B2,
};

constexpr std::array<unsigned int, num_chains> colorsJmolHeteroHex = {
    0xFFC8C8C8, 0xFFCFA090, 0xFF98CF80, 0xFFB090CF, 0xFF70CFCF, 0xFFCF90CF, 0xFFC0C080,
    0xFF60A0CF, 0xFF7050C0, 0xFF83AEC5, 0xFFCFA700, 0xFF4C4CB5, 0xFF92B556, 0xFF2AB58A,
    0xFFBE72BE, 0xFFA1B600, 0xFF6FCF00, 0xFF619B34, 0xFFBB0000, 0xFF5B9FA5, 0xFF009400,
    0xFF0000B0, 0xFF00B0B0, 0xFFB000B0, 0xFFB0B000, 0xFF13B6E8, 0xFF3232C2,
};

constexpr std::array<glm::vec4, num_chains> colorsJmol = util::make_array<num_chains>(
    [](auto index) { return molvisutil::colorFromHex(colorsJmolHex[index]); });

constexpr std::array<glm::vec4, num_chains> colorsJmolHetero = util::make_array<num_chains>(
    [](auto index) { return molvisutil::colorFromHex(colorsJmolHeteroHex[index]); });

}  // namespace detail

constexpr ChainId chain(int index) noexcept { return static_cast<ChainId>(index); }
constexpr int index(ChainId c) noexcept { return static_cast<int>(c); }
constexpr int id(ChainId c) noexcept { return static_cast<int>(c) - 1; }
constexpr ChainId fromId(int id) noexcept {
    return static_cast<ChainId>(id % (num_chains - 1) + 1);
}
constexpr std::string_view name(ChainId c) noexcept {
    if (c == ChainId::Unknown) return "Unknown";
    constexpr std::string_view lookup = "xABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return {&lookup[index(c)], 1};
}

constexpr ChainId fromName(std::string_view name) noexcept {
    if (name.size() != 1) return ChainId::Unknown;
    const char c = (name[0] >= 'a' && name[0] <= 'z') ? name[0] - 'a' + 'A' : name[0];
    if ((c < 'A') || (c > 'Z')) return ChainId::Unknown;
    return static_cast<ChainId>(c - 'A' + 1);
}

constexpr ChainId fromFullName(std::string_view fullName) noexcept {
    const auto trimmed = util::trim(fullName);
    return fromName(trimmed.substr(0, 1));
}

constexpr vec4 color(ChainId c) noexcept { return detail::colorsJmol[index(c)]; }
constexpr vec4 color(ChainId c, Colormap map) noexcept {
    switch (map) {
        case Colormap::Jmol:
            return detail::colorsJmol[index(c)];
        case Colormap::JmolHetero:
            return detail::colorsJmolHetero[index(c)];
        default:
            return color(c);
    }
}
constexpr vec4 color(int chainId) noexcept { return color(fromId(chainId)); }
constexpr vec4 color(int chainId, Colormap map) noexcept { return color(fromId(chainId), map); }
constexpr vec4 color(std::string_view fullName) noexcept { return color(fromFullName(fullName)); }
constexpr vec4 color(std::string_view fullName, Colormap map) noexcept {
    return color(fromFullName(fullName), map);
}

}  // namespace chain

}  // namespace molvis

}  // namespace inviwo
