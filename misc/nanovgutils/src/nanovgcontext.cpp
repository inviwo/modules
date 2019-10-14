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

#include <inviwo/nanovgutils/nanovgcontext.h>

#include <warn/push>
#include <warn/ignore/all>
#include <nanovg.h>
#include <nanovg_gl.h>
#include <warn/pop>

namespace inviwo {

NanoVGContext::NanoVGContext() {
    activeNanoVGContext_ = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
}

NanoVGContext::~NanoVGContext() { nvgDeleteGL3(activeNanoVGContext_); }

const NVGcontext* NanoVGContext::getContext() const { return activeNanoVGContext_; }

void NanoVGContext::activate(int windowWidth, int windowHeight, float pixelRatio) {
    nvgBeginFrame(activeNanoVGContext_, static_cast<float>(windowWidth),
                  static_cast<float>(windowHeight), pixelRatio);
}

void NanoVGContext::activate(const size2_t& dimensions, float pixelRatio) {
    nvgBeginFrame(activeNanoVGContext_, static_cast<float>(dimensions.x),
                  static_cast<float>(dimensions.y), pixelRatio);
}

void NanoVGContext::deactivate() { nvgEndFrame(activeNanoVGContext_); }

void NanoVGContext::cancel() { nvgCancelFrame(activeNanoVGContext_); }

void NanoVGContext::moveTo(const vec2& coordinates) {
    nvgMoveTo(activeNanoVGContext_, coordinates.x, coordinates.y);
}

void NanoVGContext::lineTo(const vec2& coordinates) {
    nvgLineTo(activeNanoVGContext_, coordinates.x, coordinates.y);
}

void NanoVGContext::save() { nvgSave(activeNanoVGContext_); }

void NanoVGContext::restore() { nvgRestore(activeNanoVGContext_); }

void NanoVGContext::reset() { nvgReset(activeNanoVGContext_); }

void NanoVGContext::fill() { nvgFill(activeNanoVGContext_); }

void NanoVGContext::circle(const vec2& coordinates, float radius) {
    nvgCircle(activeNanoVGContext_, coordinates.x, coordinates.y, radius);
}

void NanoVGContext::ellipse(const vec2& coordinates, float r1, float r2) {
    nvgEllipse(activeNanoVGContext_, coordinates.x, coordinates.y, r1, r2);
}

void NanoVGContext::rect(const vec2& coordinates, const vec2& dim) {
    nvgRect(activeNanoVGContext_, coordinates.x, coordinates.y, dim.x, dim.y);
}

void NanoVGContext::beginPath() { nvgBeginPath(activeNanoVGContext_); }

void NanoVGContext::closePath() { nvgClosePath(activeNanoVGContext_); }

void NanoVGContext::fillColor(const vec4& color) {
    nvgFillColor(activeNanoVGContext_, nvgRGBA(static_cast<unsigned char>(color.r * 255.0),
                                               static_cast<unsigned char>(color.g * 255.0),
                                               static_cast<unsigned char>(color.b * 255.0),
                                               static_cast<unsigned char>(color.a * 255.0)));
}

void NanoVGContext::strokeColor(const vec4& color) {
    nvgStrokeColor(activeNanoVGContext_, nvgRGBA(static_cast<unsigned char>(color.r * 255.0),
                                                 static_cast<unsigned char>(color.g * 255.0),
                                                 static_cast<unsigned char>(color.b * 255.0),
                                                 static_cast<unsigned char>(color.a * 255.0)));
}

void NanoVGContext::stroke() { nvgStroke(activeNanoVGContext_); }

void NanoVGContext::strokePaint(const NVGpaint& paint) {
    nvgStrokePaint(activeNanoVGContext_, paint);
}

void NanoVGContext::fillPaint(const NVGpaint& paint) { nvgFillPaint(activeNanoVGContext_, paint); }

void NanoVGContext::fontSize(float size) { nvgFontSize(activeNanoVGContext_, size); }

void NanoVGContext::fontFace(const std::string& name) {
    nvgFontFace(activeNanoVGContext_, name.c_str());
}

// Horizontal align
// NVG_ALIGN_LEFT = 1 << 0,	// Default, align text horizontally to left.
// NVG_ALIGN_CENTER = 1 << 1,	// Align text horizontally to center.
// NVG_ALIGN_RIGHT = 1 << 2,	// Align text horizontally to right.
// Vertical align
// NVG_ALIGN_TOP = 1 << 3,	// Align text vertically to top.
// NVG_ALIGN_MIDDLE = 1 << 4,	// Align text vertically to middle.
// NVG_ALIGN_BOTTOM = 1 << 5,	// Align text vertically to bottom.
// NVG_ALIGN_BASELINE = 1 << 6, // Default, align text vertically to baseline.
void NanoVGContext::textAlign(const int flags) { nvgTextAlign(activeNanoVGContext_, flags); }

void NanoVGContext::textAlign(const Alignment flags) {
    nvgTextAlign(activeNanoVGContext_, static_cast<int>(flags));
}

void NanoVGContext::fontBlur(float blur) { nvgFontBlur(activeNanoVGContext_, blur); }

void NanoVGContext::text(const ivec2& coordinates, const std::string& text, size_t text_cutoff) {
    if (text_cutoff == 0) {
        nvgText(activeNanoVGContext_, static_cast<float>(coordinates.x),
                static_cast<float>(coordinates.y), text.c_str(), nullptr);
    } else {
        std::string substr = text.substr(0, text_cutoff);
        nvgText(activeNanoVGContext_, static_cast<float>(coordinates.x),
                static_cast<float>(coordinates.y), substr.c_str(), nullptr);
    }
}

void NanoVGContext::textBox(const ivec2& coordinates, float textBoxWidth, const std::string& text,
                            size_t text_cutoff) {
    if (text_cutoff == 0) {
        nvgTextBox(activeNanoVGContext_, static_cast<float>(coordinates.x),
                   static_cast<float>(coordinates.y), textBoxWidth, text.c_str(), nullptr);
    } else {
        std::string substr = text.substr(0, text_cutoff);
        nvgTextBox(activeNanoVGContext_, static_cast<float>(coordinates.x),
                   static_cast<float>(coordinates.y), textBoxWidth, substr.c_str(), nullptr);
    }
}

int NanoVGContext::createFont(const std::string& name, std::string filename) {
    auto res = nvgCreateFont(activeNanoVGContext_, name.c_str(), filename.c_str());
    if (res == -1) {
        std::cout << "Font " << name << " not found at " << filename << std::endl;
    }
    return res;
}

void NanoVGContext::strokeWidth(float width) { nvgStrokeWidth(activeNanoVGContext_, width); }

vec4 NanoVGContext::textBounds(const ivec2& position, const std::string& string) {
    float bounds[4];
    nvgTextBounds(activeNanoVGContext_, static_cast<float>(position.x),
                  static_cast<float>(position.y), string.c_str(),
                  &string.c_str()[string.size() - 1], bounds);
    return vec4(bounds[0], bounds[1], bounds[2], bounds[3]);
}

vec4 NanoVGContext::textBoxBounds(const ivec2& position, float textBoxWidth,
                                  const std::string& string) {
    float bounds[4];
    nvgTextBoxBounds(activeNanoVGContext_, static_cast<float>(position.x),
                     static_cast<float>(position.y), textBoxWidth, string.c_str(),
                     &string.c_str()[string.size() - 1], bounds);
    return vec4(bounds[0], bounds[1], bounds[2], bounds[3]);
}

void NanoVGContext::arc(const vec2& center, float radius, float angle1, float angle2,
                        int direction) {
    nvgArc(activeNanoVGContext_, center.x, center.y, radius, angle1, angle2, direction);
}

void NanoVGContext::arcTo(const vec2& point1, const vec2& point2, float radius) {
    nvgArcTo(activeNanoVGContext_, point1.x, point1.y, point2.x, point2.y, radius);
}

void NanoVGContext::bezierCurveTo(const vec2& controlPoint1, const vec2& controlPoint2,
                                  const vec2& endPoint) {
    nvgBezierTo(activeNanoVGContext_, controlPoint1.x, controlPoint1.y, controlPoint2.x,
                controlPoint2.y, endPoint.x, endPoint.y);
}

void NanoVGContext::quadraticCurveTo(const vec2& controlPoint, const vec2& endPoint) {
    nvgQuadTo(activeNanoVGContext_, controlPoint.x, controlPoint.y, endPoint.x, endPoint.y);
}

void NanoVGContext::pathWinding(Winding direction) {
    nvgPathWinding(activeNanoVGContext_, static_cast<int>(direction));
}

int NanoVGContext::findFont(const std::string& name) {
    return nvgFindFont(activeNanoVGContext_, name.c_str());
}

NVGpaint NanoVGContext::linearGradient(const vec2& start, const vec2& end, const vec4& startColor,
                                       const vec4& endColor) {
    return nvgLinearGradient(activeNanoVGContext_, start.x, start.y, end.x, end.y,
                             glm2nanovg(startColor), glm2nanovg(endColor));
}

NVGpaint NanoVGContext::boxGradient(const vec2& origin, const vec2& dimensions, const float r,
                                    const float f, const vec4& startColor, const vec4& endColor) {
    return nvgBoxGradient(activeNanoVGContext_, origin.x, origin.y, dimensions.x, dimensions.y, r,
                          f, glm2nanovg(startColor), glm2nanovg(endColor));
}

NVGpaint NanoVGContext::radialGradient(const vec2& center, const float innerRadius,
                                       const float outerRadius, const vec4& startColor,
                                       const vec4& endColor) {
    return nvgRadialGradient(activeNanoVGContext_, center.x, center.y, innerRadius, outerRadius,
                             glm2nanovg(startColor), glm2nanovg(endColor));
}

void NanoVGContext::lineCap(const LineCapMode mode) {
    nvgLineCap(activeNanoVGContext_, static_cast<int>(mode));
}

void NanoVGContext::lineJoin(const LineJoinMode mode) {
    nvgLineJoin(activeNanoVGContext_, static_cast<int>(mode));
}

void NanoVGContext::transform(float a, float b, float c, float d, float e, float f) {
    nvgTransform(activeNanoVGContext_, a, b, c, d, e, f);
}

void NanoVGContext::translate(float tx, float ty) { nvgTranslate(activeNanoVGContext_, tx, ty); }

void NanoVGContext::rotate(const float angle) { nvgRotate(activeNanoVGContext_, angle); }

void NanoVGContext::scale(float sx, float sy) { nvgScale(activeNanoVGContext_, sx, sy); }

void NanoVGContext::skewX(float kx) { nvgSkewX(activeNanoVGContext_, kx); }

void NanoVGContext::skewY(float ky) { nvgSkewY(activeNanoVGContext_, ky); }

void NanoVGContext::arrow(const vec2& from, const vec2& to, float width, float headSize,
                          bool normalize) {
    moveTo(from);
    lineTo(to);

    auto dir = from - to;
    if (normalize) dir = glm::normalize(dir);
    auto up = mat2(0, 1, -1, 0) * dir;

    auto p = to + dir * width;

    moveTo(p + up * headSize);
    lineTo(to);
    lineTo(p - up * headSize);
}

vec4 NanoVGContext::nanovg2glm(const NVGcolor& col) { return vec4(col.r, col.g, col.b, col.a); }

NVGcolor NanoVGContext::glm2nanovg(const vec4& col) {
    NVGcolor color;

    color.r = col.r;
    color.g = col.g;
    color.b = col.b;
    color.a = col.a;

    return color;
}

vec2 NanoVGContext::screen2nanovg(const vec2& coord, const size2_t& dimensions) {
    return {coord.x, float(dimensions.y) - coord.y};
}

vec2 NanoVGContext::getQuadraticBezierCurveControlPoint(const vec2& p0, const vec2& p1,
                                                        const vec2& p2, const float t) {
    return t * t * p0 + t * p1 + t * t * p2;
}

std::vector<vec2> NanoVGContext::getQuadraticBezierCurveControlPoints(
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

std::pair<std::vector<vec2>, std::vector<vec2>> NanoVGContext::getCubicBezierCurveControlPoints(
    const std::vector<vec2>& knots) {
    std::pair<std::vector<vec2>, std::vector<vec2>> controlPoints{};
    auto& firstControlPoints = controlPoints.first;
    auto& secondControlPoints = controlPoints.second;

    const auto n = knots.size() - 1;

    if (n < 1) {
        LogError("At least two knot points required");
        return controlPoints;
    }

    if (n == 1) {  // Special case: Bezier curve should be a straight line.
        firstControlPoints.emplace_back();
        // 3P1 = 2P0 + P3
        firstControlPoints[0].x = (2.0f * knots[0].x + knots[1].x) / 3.0f;
        firstControlPoints[0].y = (2.0f * knots[0].y + knots[1].y) / 3.0f;

        secondControlPoints.emplace_back();
        // P2 = 2P1 – P0
        secondControlPoints[0].x = 2.0f * firstControlPoints[0].x - knots[0].x;
        secondControlPoints[0].y = 2.0f * firstControlPoints[0].y - knots[0].y;
        return controlPoints;
    }

    // Calculate first Bezier control points
    // Right hand side vector
    auto rhs = std::vector<float>();
    rhs.resize(n);

    // Set right hand side X values
    for (size_t i = 1; i < n - 1; ++i) {
        rhs[i] = 4.0f * knots[i].x + 2.0f * knots[i + 1].x;
    }
    rhs[0] = knots[0].x + 2.0f * knots[1].x;
    rhs[n - 1] = (8.0f * knots[n - 1].x + knots[n].x) / 2.0f;
    // Get first control points X-values
    auto x = GetFirstControlPoints(rhs);

    // Set right hand side Y values
    for (size_t i = 1; i < n - 1; ++i) {
        rhs[i] = 4 * knots[i].y + 2 * knots[i + 1].y;
    }
    rhs[0] = knots[0].y + 2.0f * knots[1].y;
    rhs[n - 1] = (8.0f * knots[n - 1].y + knots[n].y) / 2.0f;
    // Get first control points Y-values
    auto y = GetFirstControlPoints(rhs);

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

std::vector<float> NanoVGContext::GetFirstControlPoints(const std::vector<float>& rhs) {
    const auto n = rhs.size();

    auto x = std::vector<float>();  // Solution vector.
    x.resize(n);

    auto tmp = std::vector<float>();  // Temp workspace.
    tmp.resize(n);

    auto b = 2.0f;
    x[0] = rhs[0] / b;
    for (size_t i = 1; i < n; i++)  // Decomposition and forward substitution.
    {
        tmp[i] = 1.0f / b;
        b = (i < n - 1 ? 4.0f : 3.5f) - tmp[i];
        x[i] = (rhs[i] - x[i - 1]) / b;
    }
    for (size_t i = 1; i < n; i++) {
        x[n - i - 1] -= tmp[n - i] * x[n - i];  // Backsubstitution.
    }

    return x;
}

}  // namespace inviwo
