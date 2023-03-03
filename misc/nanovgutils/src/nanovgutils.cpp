/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2023 Inviwo Foundation
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

#include <inviwo/nanovgutils/nanovgutils.h>

#include <inviwo/nanovgutils/nanovgutilsmodule.h>

#include <inviwo/core/util/moduleutils.h>

namespace inviwo::nanovgutil {

NanoVGContext& getContext(InviwoApplication* app) {
    return util::getModuleByType<NanoVGUtilsModule>(app)->getNanoVGContext();
}

NanoVGContext& getContext() {
    return util::getModuleByType<NanoVGUtilsModule>()->getNanoVGContext();
}

void arrow(NanoVGContext& ctx, const vec2& from, const vec2& to, float width, float headSize,
           bool normalize) {

    ctx.moveTo(from);
    ctx.lineTo(to);

    auto dir = from - to;
    if (normalize) dir = glm::normalize(dir);
    auto up = mat2(0, 1, -1, 0) * dir;

    auto p = to + dir * width;

    ctx.moveTo(p + up * headSize);
    ctx.lineTo(to);
    ctx.lineTo(p - up * headSize);
}

vec2 getQuadraticBezierCurveControlPoint(const vec2& p0, const vec2& p1, const vec2& p2,
                                         const float t) {
    return t * t * p0 + t * p1 + t * t * p2;
}

std::vector<vec2> getQuadraticBezierCurveControlPoints(
    const std::vector<vec2>& startAndEndPoints, const std::vector<vec2>& intermediatePoints) {
    std::vector<vec2> controlPoints{};

    const auto n = startAndEndPoints.size();

    if (n < 1) {
        LogErrorCustom("nanovgutils.cpp", "At least two knot points required.");
        return controlPoints;
    }

    const auto m = intermediatePoints.size();

    if (m != n - 1) {
        LogErrorCustom("nanovgutils.cpp", "Requirement n-1 == m not satisfied.");
        return controlPoints;
    }

    for (size_t i = 0; i < n - 1; ++i) {
        const auto& p0 = startAndEndPoints[i];
        const auto& p1 = intermediatePoints[i];
        const auto& p2 = startAndEndPoints[i + 1];

        controlPoints.emplace_back(getQuadraticBezierCurveControlPoint(p0, p1, p2));
    }

    return controlPoints;
}

std::pair<std::vector<vec2>, std::vector<vec2>> getCubicBezierCurveControlPoints(
    const std::vector<vec2>& knots) {
    std::pair<std::vector<vec2>, std::vector<vec2>> controlPoints{};
    auto& firstControlPoints = controlPoints.first;
    auto& secondControlPoints = controlPoints.second;

    const auto n = knots.size() - 1;

    if (n < 1) {
        LogErrorCustom("getCubicBezierCurveControlPoints", "At least two knot points required");
        return controlPoints;
    }

    if (n == 1) {  // Special case: Bezier curve should be a straight line.
        firstControlPoints.emplace_back();
        // 3P1 = 2P0 + P3
        firstControlPoints[0].x = (2.0f * knots[0].x + knots[1].x) / 3.0f;
        firstControlPoints[0].y = (2.0f * knots[0].y + knots[1].y) / 3.0f;

        secondControlPoints.emplace_back();
        // P2 = 2P1 - P0
        secondControlPoints[0].x = 2.0f * firstControlPoints[0].x - knots[0].x;
        secondControlPoints[0].y = 2.0f * firstControlPoints[0].y - knots[0].y;
        return controlPoints;
    }

    // Calculate first Bezier control points
    // Right hand side vector
    auto rhs = std::vector<float>(n);

    // Set right hand side X values
    for (size_t i = 1; i < n - 1; ++i) {
        rhs[i] = 4.0f * knots[i].x + 2.0f * knots[i + 1].x;
    }
    rhs[0] = knots[0].x + 2.0f * knots[1].x;
    rhs[n - 1] = (8.0f * knots[n - 1].x + knots[n].x) / 2.0f;
    // Get first control points X-values
    auto x = getFirstControlPoints(rhs);

    // Set right hand side Y values
    for (size_t i = 1; i < n - 1; ++i) {
        rhs[i] = 4 * knots[i].y + 2 * knots[i + 1].y;
    }
    rhs[0] = knots[0].y + 2.0f * knots[1].y;
    rhs[n - 1] = (8.0f * knots[n - 1].y + knots[n].y) / 2.0f;
    // Get first control points Y-values
    auto y = getFirstControlPoints(rhs);

    // Fill output arrays.
    firstControlPoints.resize(n);
    secondControlPoints.resize(n);
    for (size_t i = 0; i < n; ++i) {
        // First control point
        firstControlPoints[i] = vec2(x[i], y[i]);
        // Second control point
        if (i < n - 1)
            secondControlPoints[i] =
                vec2(2.0f * knots[i + 1].x - x[i + 1], 2.0f * knots[i + 1].y - y[i + 1]);
        else
            secondControlPoints[i] =
                vec2((knots[n].x + x[n - 1]) / 2.0f, (knots[n].y + y[n - 1]) / 2.0f);
    }

    return controlPoints;
}

std::vector<float> getFirstControlPoints(const std::vector<float>& rhs) {
    const auto n = rhs.size();

    auto solution = std::vector<float>(n);  // Solution vector.
    auto tmp = std::vector<float>(n);       // Temp workspace.

    auto b = 2.0f;
    solution[0] = rhs[0] / b;
    for (size_t i = 1; i < n; i++)  // Decomposition and forward substitution.
    {
        tmp[i] = 1.0f / b;
        b = (i < n - 1 ? 4.0f : 3.5f) - tmp[i];
        solution[i] = (rhs[i] - solution[i - 1]) / b;
    }
    for (size_t i = 1; i < n; i++) {
        solution[n - i - 1] -= tmp[n - i] * solution[n - i];  // Backsubstitution.
    }

    return solution;
}

void setFontProperties(NanoVGContext& nvg, const NanoVGFontProperty& properties) {
    nvg.fillColor(properties.getFontColor());
    nvg.fontSize(properties.getFontSize());
    nvg.fontFace(properties.getFontFace());
    nvg.textAlign(properties.getFontAligntment());
    if (const auto fb = properties.getFontBlur()) {
        nvg.fontBlur(*fb);
    }
}

void setFontProperties(NanoVGContext& nvg, const float fontSize, const vec4& fontColor,
                       const std::string& fontFace, NanoVGContext::Alignment fontAlignment,
                       const float fontBlurIntensity) {
    nvg.fillColor(fontColor);
    nvg.fontSize(fontSize);
    nvg.fontFace(fontFace);
    nvg.textAlign(fontAlignment);
    nvg.fontBlur(fontBlurIntensity);
}

}  // namespace inviwo::nanovgutil
