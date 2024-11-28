/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2024 Inviwo Foundation
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

#include <inviwo/nanovgutils/nanovgutils.h>

#include <warn/push>
#include <warn/ignore/all>
#include <nanovg.h>


#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>
#include <warn/pop>

namespace inviwo {

using namespace inviwo::nanovgutil;

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

void NanoVGContext::shapeAntiAlias(bool enabled) {
    nvgShapeAntiAlias(activeNanoVGContext_, enabled);
}

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

void NanoVGContext::roundedRect(const vec2& coordinates, const vec2& dim, float r) {
    nvgRoundedRect(activeNanoVGContext_, coordinates.x, coordinates.y, dim.x, dim.y, r);
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

int NanoVGContext::createFont(const std::string& name, const std::filesystem::path& filename) {
    return nvgCreateFont(activeNanoVGContext_, name.c_str(), filename.string().c_str());
}

void NanoVGContext::strokeWidth(float width) { nvgStrokeWidth(activeNanoVGContext_, width); }

vec4 NanoVGContext::textBounds(const ivec2& position, const std::string& string) {
    float bounds[4];
    nvgTextBounds(activeNanoVGContext_, static_cast<float>(position.x),
                  static_cast<float>(position.y), string.c_str(), nullptr, bounds);
    return vec4(bounds[0], bounds[1], bounds[2], bounds[3]);
}

vec4 NanoVGContext::textBounds(const vec2& position, const std::string& string) {
    float bounds[4];
    nvgTextBounds(activeNanoVGContext_, position.x, position.y, string.c_str(), nullptr, bounds);
    return vec4(bounds[0], bounds[1], bounds[2], bounds[3]);
}

vec4 NanoVGContext::textBoxBounds(const ivec2& position, float textBoxWidth,
                                  const std::string& string) {
    float bounds[4];
    nvgTextBoxBounds(activeNanoVGContext_, static_cast<float>(position.x),
                     static_cast<float>(position.y), textBoxWidth, string.c_str(), nullptr, bounds);
    return vec4(bounds[0], bounds[1], bounds[2], bounds[3]);
}

vec3 NanoVGContext::textMetrics() {
    vec3 res;
    nvgTextMetrics(activeNanoVGContext_, &res.x, &res.y, &res.z);
    return res;
}

float NanoVGContext::textMetricsAscender() { return textMetrics().x; }
float NanoVGContext::textMetricsDescender() { return textMetrics().y; }
float NanoVGContext::textMetricsLineHeight() { return textMetrics().z; }

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

}  // namespace inviwo
