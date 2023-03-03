/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2023 Inviwo Foundation
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
#include <inviwo/core/util/glm.h>

#include <glm/ext/scalar_constants.hpp>

#include <string_view>

namespace inviwo {

namespace molvisutil {

/**
 * \brief convert an RGBA hex color to vec4
 *
 * @param c    color stored as RGBA from least to most significant bit
 * @return rgba color with range [0,1]
 */
constexpr vec4 colorFromHex(unsigned int c) noexcept {
    return vec4{(c >> 0) & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff, (c >> 24) & 0xff} / 255.0f;
}

namespace detail {

constexpr vec3 hclToRgb(vec3 hcl) noexcept {
    constexpr float hclgamma = 3;
    constexpr float hcly0 = 100;
    constexpr float hclmaxL = 0.530454533953517f;  // == exp(hclgamma / hcly0) - 0.5

    vec3 rgb = vec3(0);
    if (hcl.z != 0) {
        float H = hcl.x;
        float C = hcl.y;
        float L = hcl.z * hclmaxL;
        float Q = exp((1 - C / (2 * L)) * (hclgamma / hcly0));
        float U = (2 * L - C) / (2 * Q - 1);
        float V = C / Q;
        float T = tan((H + glm::min(glm::fract(2.0f * H) / 4.f, glm::fract(-2.0f * H) / 8.0f)) *
                      glm::pi<float>() * 2.0f);
        H *= 6.0f;
        if (H <= 1) {
            rgb.r = 1;
            rgb.g = T / (1 + T);
        } else if (H <= 2) {
            rgb.r = (1 + T) / T;
            rgb.g = 1;
        } else if (H <= 3) {
            rgb.g = 1;
            rgb.b = 1 + T;
        } else if (H <= 4) {
            rgb.g = 1 / (1 + T);
            rgb.b = 1;
        } else if (H <= 5) {
            rgb.r = -1 / T;
            rgb.b = 1;
        } else {
            rgb.r = 1;
            rgb.b = -T;
        }
        rgb = rgb * V + U;
    }
    return rgb;
}

}  // namespace detail

constexpr vec4 colorFromHash(uint64_t hash) {
    const float chroma = 0.8f;
    const float luminance = 0.90f;
    const uint64_t mod = 21;
    const float hue = (hash % mod) / (float)mod;
    return vec4(detail::hclToRgb({hue, chroma, luminance}), 1.0f);
}

}  // namespace molvisutil

}  // namespace inviwo
