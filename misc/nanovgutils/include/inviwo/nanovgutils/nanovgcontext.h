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

#include <inviwo/core/common/inviwo.h>
#include <inviwo/nanovgutils/nanovgutilsmoduledefine.h>

#include <GL/glew.h>

#include <warn/push>
#include <warn/ignore/all>
#include <nanovg.h>
#include <warn/pop>

namespace inviwo {
class IVW_MODULE_NANOVGUTILS_API NanoVGContext {
    friend class NanoVGUtilsModule;
    explicit NanoVGContext();
public:
    NanoVGContext(const NanoVGContext &) = delete;
    NanoVGContext(NanoVGContext &&) = delete;
    NanoVGContext &operator=(const NanoVGContext &) = delete;
    NanoVGContext &operator=(NanoVGContext &&) = delete;
    virtual ~NanoVGContext();

    enum class LineCapMode {
        Butt = NVG_BUTT,
        Round = NVG_ROUND,
        Square = NVG_SQUARE,
    };

    enum class LineJoinMode {
        Miter = NVG_MITER,
        Round = NVG_ROUND,
        Bevel = NVG_BEVEL,
    };

    enum class Winding {
        CCW = NVG_CCW,
        CW = NVG_CW,
    };

    enum class Alignment {
        Left_Top = (1 << 0 | 1 << 3),
        Left_Middle = (1 << 0 | 1 << 4),
        Left_Bottom = (1 << 0 | 1 << 5),
        Left_Baseline = (1 << 0 | 1 << 6),
        Center_Top = (1 << 1 | 1 << 3),
        Center_Middle = (1 << 1 | 1 << 4),
        Center_Bottom = (1 << 1 | 1 << 5),
        Center_Baseline = (1 << 1 | 1 << 6),
        Right_Top = (1 << 2 | 1 << 3),
        Right_Middle = (1 << 2 | 1 << 4),
        Right_Bottom = (1 << 2 | 1 << 5),
        Right_Baselin = (1 << 2 | 1 << 6)
    };

    /**********************************************************************************************
     * NanoVG setup
     **********************************************************************************************/

    // Begin drawing a new frame
    // Calls to nanovg drawing API should be wrapped in nvgBeginFrame() & nvgEndFrame()
    // nvgBeginFrame() defines the size of the window to render to in relation currently
    // set viewport (i.e. glViewport on GL backends). Device pixel ration allows to
    // control the rendering on Hi-DPI devices.
    // For example, GLFW returns two dimension for an opened window: window size and
    // frame buffer size. In that case you would set windowWidth/Height to the window size
    // devicePixelRatio to: frameBufferWidth / windowWidth.
    void activate(int windowWidth, int windowHeight, float pixelRatio = 1.f);
    // Begin drawing a new frame
    // Calls to nanovg drawing API should be wrapped in nvgBeginFrame() & nvgEndFrame()
    // nvgBeginFrame() defines the size of the window to render to in relation currently
    // set viewport (i.e. glViewport on GL backends). Device pixel ration allows to
    // control the rendering on Hi-DPI devices.
    // For example, GLFW returns two dimension for an opened window: window size and
    // frame buffer size. In that case you would set windowWidth/Height to the window size
    // devicePixelRatio to: frameBufferWidth / windowWidth.
    void activate(const size2_t &dimensions, float pixelRatio = 1.f);
    // Ends drawing flushing remaining render state.
    void deactivate();
    // Cancels drawing the current frame.
    void cancel();
    // Returns the currently active NanoVG context
    const NVGcontext *getContext() const;

    /**********************************************************************************************
     * NanoVG states
     **********************************************************************************************/

    // Pushes and saves the current render state into a state stack.
    // A matching nvgRestore() must be used to restore the state.
    void save();
    // Pops and restores current render state.
    void restore();
    // Resets current render state to default values. Does not affect the render state stack.
    void reset();

    /**********************************************************************************************
     * NanoVG paths
     **********************************************************************************************/

    // Clears the current path and sub-paths.
    void beginPath();
    // Closes current sub-path with a line segment.
    void closePath();
    // Sets the current sub-path winding.
    void pathWinding(Winding direction);
    // Sets how the end of the line (cap) is drawn,
    // Can be one of: Butt (default), Round, Square.
    void lineCap(LineCapMode mode = LineCapMode::Butt);
    // Sets how sharp path corners are drawn.
    // Can be one of Miter (default), Round, Bevel.
    void lineJoin(LineJoinMode mode = LineJoinMode::Miter);
    // Starts new sub-path with specified point as first point.
    void moveTo(const vec2 &coordinates);
    // Adds line segment from the last point in the path to the specified point.
    void lineTo(const vec2 &coordinates);
    // Creates new circle shaped sub-path.
    void circle(const vec2 &coordinates, float radius);
    // Creates new ellipse shaped sub-path.
    void ellipse(const vec2 &coordinates, float radius1, float radius2);
    // Creates new rectangle shaped sub-path.
    void rect(const vec2 &coordinates, const vec2 &dim);
    // Creates new circle arc shaped sub-path. The arc center is at cx,cy, the arc radius is r,
    // and the arc is drawn from angle a0 to a1, and swept in direction dir (NVG_CCW, or NVG_CW).
    // Angles are specified in radians.
    void arc(const vec2 &center, float radius, float angle1, float angle2, int direction);
    // Adds an arc segment at the corner defined by the last path point, and two specified points.
    void arcTo(const vec2 &point1, const vec2 &point2, float radius);
    // Adds cubic bezier segment from last point in the path via two control points to the specified
    // point.
    void bezierCurveTo(const vec2 &controlPoint1, const vec2 &controlPoint2, const vec2 &endPoint);
    // Adds quadratic bezier segment from last point in the path via a control point to the
    // specified point.
    void quadraticCurveTo(const vec2 &controlPoint, const vec2 &endPoint);

    /**********************************************************************************************
     * NanoVG coloring
     **********************************************************************************************/

    // Sets current fill style to a solid color.
    void fillColor(const vec4 &color);
    // Fills the current path with current fill style.
    void fill();
    // Sets current stroke style to a solid color.
    void strokeColor(const vec4 &color);
    // Sets the stroke width of the stroke style.
    void strokeWidth(float width);
    // Fills the current path with current stroke style.
    void stroke();
    // Sets current stroke style to a paint, which can be a one of the gradients or a pattern.
    void strokePaint(const NVGpaint &paint);
    // Sets current fill style to a paint, which can be a one of the gradients or a pattern.
    void fillPaint(const NVGpaint &paint);

    /**********************************************************************************************
     * NanoVG fonts and text
     **********************************************************************************************/

    // Sets the font size of current text style.
    void fontSize(float size);
    // Sets the font face based on specified name of current text style.
    void fontFace(const std::string &name);
    // Sets the text align of current text style.
    // Horizontal align
    // NVG_ALIGN_LEFT = 1 << 0,	// Default, align text horizontally to left.
    // NVG_ALIGN_CENTER = 1 << 1,	// Align text horizontally to center.
    // NVG_ALIGN_RIGHT = 1 << 2,	// Align text horizontally to right.
    // Vertical align
    // NVG_ALIGN_TOP = 1 << 3,	// Align text vertically to top.
    // NVG_ALIGN_MIDDLE = 1 << 4,	// Align text vertically to middle.
    // NVG_ALIGN_BOTTOM = 1 << 5,	// Align text vertically to bottom.
    // NVG_ALIGN_BASELINE = 1 << 6, // Default, align text vertically to baseline.
    void textAlign(int flags);
    // Sets the text align of current text style.
    void textAlign(Alignment flags);
    // Sets the blur of current text style.
    void fontBlur(float blur);
    // Draws text string at specified location. If end is specified only the sub-string up to the
    // end is drawn.
    void text(const ivec2 &coordinates, const std::string &text, size_t text_cutoff = 0);
    // Draws multi-line text string at specified location wrapped at the specified width. If end is
    // specified only the sub-string up to the end is drawn. White space is stripped at the
    // beginning of the rows, the text is split at word boundaries or when new-line characters are
    // encountered. Words longer than the max width are slit at nearest character (i.e. no
    // hyphenation).
    void textBox(const ivec2 &coordinates, float textBoxWidth, const std::string &text,
                 size_t text_cutoff = 0);
    // Creates font by loading it from the disk from specified file name.
    // Returns handle to the font.
    int createFont(const std::string &name, std::string filename);
    // Measures the specified text string. Parameter bounds should be a pointer to float[4],
    // if the bounding box of the text should be returned. The bounds value are [xmin,ymin,
    // xmax,ymax] Returns the horizontal advance of the measured text (i.e. where the next character
    // should drawn). Measured values are returned in local coordinate space.
    vec4 textBounds(const ivec2 &position, const std::string &string);
    // Measures the specified multi-text string. Parameter bounds should be a pointer to float[4],
    // if the bounding box of the text should be returned. The bounds value are [xmin,ymin,
    // xmax,ymax] Measured values are returned in local coordinate space.
    vec4 textBoxBounds(const ivec2 &position, float textBoxWidth, const std::string &text);
    // Finds a loaded font of specified name, and returns handle to it, or -1 if the font is not
    // found.
    int findFont(const std::string &name);

    /**********************************************************************************************
     * NanoVG gradients
     **********************************************************************************************/

    // Creates and returns a linear gradient. Parameters start-end specify the start and end
    // coordinates of the linear gradient, startColor specifies the start color and endColor the end
    // color. The gradient is transformed by the current transform when it is passed to
    // fillPaint() or strokePaint().
    NVGpaint linearGradient(const vec2 &start, const vec2 &end, const vec4 &startColor,
                            const vec4 &endColor);

    // Creates and returns a box gradient. Box gradient is a feathered rounded rectangle, it is
    // useful for rendering drop shadows or highlights for boxes. Parameter origin defines the
    // top-left corner of the rectangle, dimensions defines the size of the rectangle, r defines the
    // corner radius, and f feather. Feather defines how blurry the border of the rectangle is.
    // Parameter icol specifies the inner color and ocol the outer color of the gradient. The
    // gradient is transformed by the current transform when it is passed to fillPaint() or
    // strokePaint().
    NVGpaint boxGradient(const vec2 &origin, const vec2 &dimensions, float r, float f,
                         const vec4 &startColor, const vec4 &endColor);

    // Creates and returns a radial gradient. Parameter center specifies the center, innerRadius and
    // outerRadius specify the inner and outer radius of the gradient, startColor specifies the
    // start color and endColor the end color. The gradient is transformed by the current transform
    // when it is passed to fillPaint() or strokePaint().
    NVGpaint radialGradient(const vec2 &center, float innerRadius, float outerRadius,
                            const vec4 &startColor, const vec4 &endColor);

    /**********************************************************************************************
     * NanoVG transformations
     **********************************************************************************************/

    // transformations in nanovg
    // [sx kx tx]
    // [ky sy ty]
    // [ 0  0  1]
    // Where: sx,sy define scaling, kx,ky skewing, and tx,ty translation.
    // The parameters are interpreted as matrix as follows:
    //   [a c e]
    //   [b d f]
    //   [0 0 1]
    void transform(float a, float b, float c, float d, float e, float f);
    // Translates current coordinate system.
    void translate(float tx, float ty);
    // Rotates current coordinate system. Angle is specified in radians.
    void rotate(float angle);
    // Scales the current coordinate system.
    void scale(float sx, float sy);
    // Skews the current coordinate system along X axis. Angle is specified in radians.
    void skewX(float kx);
    // Skews the current coordinate system along Y axis. Angle is specified in radians.
    void skewY(float ky);

    /**********************************************************************************************
     * NanoVG shape utilities
     **********************************************************************************************/

    void arrow(const vec2 &from, const vec2 &to, float width = 30, float headSize = 30,
               bool normalize = true);

    // Get quadratic Bezier Spline Control Point.
    vec2 getQuadraticBezierCurveControlPoint(const vec2 &p0, const vec2 &p1, const vec2 &p2,
                                             float t = 0.5f);

    // Get quadratic Bezier Spline Control Points.
    std::vector<vec2> getQuadraticBezierCurveControlPoints(
        const std::vector<vec2> &startAndEndPoints, const std::vector<vec2> &intermediatePoints);

    // Get cubic Bezier Spline Control Points.
    // https://www.codeproject.com/Articles/31859/Draw-a-Smooth-Curve-through-a-Set-of-2D-Points-wit
    std::pair<std::vector<vec2>, std::vector<vec2>> getCubicBezierCurveControlPoints(
        const std::vector<vec2> &knots);

    // Solves a tridiagonal system for one of coordinates (x or y) of first Bezier control points.
    std::vector<float> GetFirstControlPoints(const std::vector<float> &rhs);

    /**********************************************************************************************
     * NanoVG converters
     **********************************************************************************************/
    vec4 nanovg2glm(const NVGcolor &col);
    NVGcolor glm2nanovg(const vec4 &col);

    static vec2 screen2nanovg(const vec2 &coord, const size2_t &dimensions);

private:
    NVGcontext *activeNanoVGContext_;

};  // NVG

}  // namespace inviwo