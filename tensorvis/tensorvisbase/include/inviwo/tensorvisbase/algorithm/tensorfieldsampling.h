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

#pragma once

#include <inviwo/tensorvisbase/datastructures/tensorfield2d.h>
#include <inviwo/tensorvisbase/datastructures/tensorfield3d.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>

namespace inviwo {
namespace tensorutil {
enum class InterpolationMethod { Linear, Nearest, Barycentric };
}

template <tensorutil::InterpolationMethod method>
dmat2 sample(std::shared_ptr<const TensorField2D> tensorField, const dvec2& position) {
    const auto fBounds = tensorField->getBounds<double>();

    if constexpr (method == tensorutil::InterpolationMethod::Nearest) {
        return tensorField->at(size2_t(glm::round(position * dvec2(fBounds))));
    }

    const auto uBounds = tensorField->getBounds();
    const auto indexPosition = size2_t(glm::floor(position * dvec2(fBounds)));

    const auto pos00 = size2_t(indexPosition);
    const auto pos11 = pos00 + size2_t(1);
    const auto pos10 = pos00 + size2_t(1, 0);
    const auto pos01 = pos00 + size2_t(0, 1);

    const auto xFrac = glm::fract(position.x * fBounds.x);
    const auto yFrac = glm::fract(position.y * fBounds.y);

    if (pos00 == uBounds) {
        return tensorField->at(uBounds);
    }
    if (pos00.x == uBounds.x) {
        return glm::mix(tensorField->at(pos00), tensorField->at(pos01), yFrac);
    }
    if (pos00.y == uBounds.y) {
        return glm::mix(tensorField->at(pos00), tensorField->at(pos10), xFrac);
    }

    if constexpr (method == tensorutil::InterpolationMethod::Linear) {
        const auto& tensor00 = tensorField->at(pos00);
        const auto& tensor11 = tensorField->at(pos11);
        const auto& tensor10 = tensorField->at(pos10);
        const auto& tensor01 = tensorField->at(pos01);

        const auto botRow = glm::mix(tensor00, tensor10, xFrac);
        const auto topRow = glm::mix(tensor01, tensor11, xFrac);

        return glm::mix(botRow, topRow, yFrac);
    }
    if constexpr (method == tensorutil::InterpolationMethod::Barycentric) {
        // Find out which triangle we need to use
        auto a = dvec2(pos00) / fBounds;
        auto b = dvec2(pos11) / fBounds;
        auto c = position;

        dmat2 tensorAtA;
        dmat2 tensorAtB;
        dmat2 tensorAtC;

        auto sys = dmat2(a.x - c.x, a.y - c.y, b.x - c.x, b.y - c.y);
        // lower
        if (glm::determinant(sys) <= 0.) {
            a = dvec2(pos00) / fBounds;
            tensorAtA = tensorField->at(pos00);
            b = dvec2(pos10) / fBounds;
            tensorAtB = tensorField->at(pos10);
            c = dvec2(pos11) / fBounds;
            tensorAtC = tensorField->at(pos11);
        }
        // upper
        else {
            a = dvec2(pos00) / fBounds;
            tensorAtA = tensorField->at(pos00);
            b = dvec2(pos11) / fBounds;
            tensorAtB = tensorField->at(pos11);
            c = dvec2(pos01) / fBounds;
            tensorAtC = tensorField->at(pos01);
        }

        // barycentric coords
        const auto v0 = b - a;
        const auto v1 = c - a;
        const auto v2 = position - a;
        const auto den = v0.x * v1.y - v1.x * v0.y;
        const auto v = (v2.x * v1.y - v1.x * v2.y) / den;
        const auto w = (v0.x * v2.y - v2.x * v0.y) / den;
        const auto u = 1.0f - v - w;

        return dmat2(u * tensorAtA + v * tensorAtB + w * tensorAtC);
    }
    return dmat2();
}

IVW_MODULE_TENSORVISBASE_API dmat2 sample(std::shared_ptr<const TensorField2D> tensorField,
                                          const dvec2& position,
                                          const tensorutil::InterpolationMethod method);

/**
 * Returns a pair of a glm::uint8 and dmat3.
 * The dmat3 is the tensor. Since the field stores tensors at every position
 * it has a mask defining where it is actually defined and where not. It is 1
 * if there is data at this position and 0 if not. If the mask value is zero,
 * the tensor will be a 0 tensor. If the mask is not set for the tensor field,
 * the mask value return will always be 0.
 */
template <tensorutil::InterpolationMethod method>
std::pair<glm::uint8, dmat3> sample(std::shared_ptr<const TensorField3D> tensorField,
                                    const dvec3& position) {
    // Position is in texture space [0,1], translate to index space
    const auto bounds = tensorField->getBounds<double>();
    const auto indexPosition = position * bounds;

    dmat3 val{0.0};

    if constexpr (method == tensorutil::InterpolationMethod::Linear) {
        const auto xm = glm::floor(indexPosition.x);
        const auto ym = glm::floor(indexPosition.y);
        const auto zm = glm::floor(indexPosition.z);

        const auto xp = glm::ceil(indexPosition.x);
        const auto yp = glm::ceil(indexPosition.y);
        const auto zp = glm::ceil(indexPosition.z);

        const auto xFrac = glm::fract(position.x * bounds.x);
        const auto yFrac = glm::fract(position.y * bounds.y);
        const auto zFrac = glm::fract(position.z * bounds.z);

        // Upper layer
        const auto& tensor010 = tensorField->at(size3_t(xm, yp, zm));
        const auto& tensor011 = tensorField->at(size3_t(xm, yp, zp));
        const auto tensorUpperLeft = glm::mix(tensor010, tensor011, zFrac);

        const auto& tensor111 = tensorField->at(size3_t(xp, yp, zp));
        const auto& tensor110 = tensorField->at(size3_t(xp, yp, zm));
        const auto tensorUpperRight = glm::mix(tensor110, tensor111, zFrac);

        const auto tensorUpper = glm::mix(tensorUpperLeft, tensorUpperRight, xFrac);

        // Lower layer
        const auto& tensor000 = tensorField->at(size3_t(xm, ym, zm));
        const auto& tensor001 = tensorField->at(size3_t(xm, ym, zp));
        const auto tensorLowerLeft = glm::mix(tensor000, tensor001, zFrac);

        const auto& tensor100 = tensorField->at(size3_t(xp, ym, zm));
        const auto& tensor101 = tensorField->at(size3_t(xp, ym, zp));
        const auto tensorLowerRight = glm::mix(tensor100, tensor101, zFrac);

        const auto tensorLower = glm::mix(tensorLowerLeft, tensorLowerRight, xFrac);

        val = glm::mix(tensorLower, tensorUpper, yFrac);
    } else {
        if constexpr (method == tensorutil::InterpolationMethod::Nearest) {
            val = tensorField->at(size3_t(glm::round(indexPosition)));
        }
    }
    return std::pair<glm::uint8, dmat3>(glm::uint8{1}, val);
}

/**
 * Returns a pair of a glm::uint8 and dmat3.
 * The dmat3 is the tensor. Since the field stores tensors at every position
 * it has a mask defining where it is actually defined and where not. It is 1
 * if there is data at this position and 0 if not. If the mask value is zero,
 * the tensor will be a 0 tensor. If the mask is not set for the tensor field,
 * the mask value return will always be 0.
 */
IVW_MODULE_TENSORVISBASE_API std::pair<glm::uint8, dmat3> sample(
    std::shared_ptr<const TensorField3D> tensorField, const dvec3& position,
    const tensorutil::InterpolationMethod method);
}  // namespace inviwo
