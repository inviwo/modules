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

#include <inviwo/nanovgutils/nanovgutilsmoduledefine.h>
#include <inviwo/core/common/inviwo.h>

#include <inviwo/nanovgutils/nanovgcontext.h>
#include <inviwo/nanovgutils/properties/nanovgfontproperty.h>

namespace inviwo::nanovgutil {

IVW_MODULE_NANOVGUTILS_API NanoVGContext& getContext(InviwoApplication* app);

IVW_MODULE_NANOVGUTILS_API NanoVGContext& getContext();

IVW_MODULE_NANOVGUTILS_API void arrow(NanoVGContext& ctx, const vec2& from, const vec2& to,
                                      float width = 30, float headSize = 30, bool normalize = true);

inline vec4 nanovg2glm(const NVGcolor& col) { return {col.r, col.g, col.b, col.a}; }
inline NVGcolor glm2nanovg(const vec4& col) { return {col.r, col.g, col.b, col.a}; }

inline vec2 screen2nanovg(const vec2& coord, const size2_t& dims) {
    return {coord.x, static_cast<float>(dims.y) - coord.y};
}

// Get quadratic Bezier Spline Control Point.
vec2 getQuadraticBezierCurveControlPoint(const vec2& p0, const vec2& p1, const vec2& p2,
                                         float t = 0.5f);

// Get quadratic Bezier Spline Control Points.
std::vector<vec2> getQuadraticBezierCurveControlPoints(const std::vector<vec2>& startAndEndPoints,
                                                       const std::vector<vec2>& intermediatePoints);

// Get cubic Bezier Spline Control Points.
// https://www.codeproject.com/Articles/31859/Draw-a-Smooth-Curve-through-a-Set-of-2D-Points-wit
std::pair<std::vector<vec2>, std::vector<vec2>> getCubicBezierCurveControlPoints(
    const std::vector<vec2>& knots);

// Solves a tridiagonal system for one of coordinates (x or y) of first Bezier control points.
std::vector<float> getFirstControlPoints(const std::vector<float>& rhs);

void setFontProperties(NanoVGContext& nvg, const NanoVGFontProperty& properties);

}  // namespace inviwo::nanovgutil
