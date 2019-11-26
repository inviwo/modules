/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2018 Inviwo Foundation
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

namespace inviwo {
namespace tensorutil {
namespace detail {
inline std::string getHTMLTableColumnString(const std::string& item) {
    return "  <td style='color:#bbb;padding-right:8px;'>"
           "  <span style=\"white-space: nowrap;\">" +
           item +
           "  </span>"
           "  </td>";
}

inline std::string getHTMLTableColumnSpanString(const std::string& item,
                                                const size_t numberOfColumnsToSpan) {
    return "  <td colspan=\"" + std::to_string(numberOfColumnsToSpan) +
           "\" style='color:#bbb;padding-right:8px;'>"
           "  <span style=\"white-space: nowrap;\">"
           "<strong>" +
           item +
           "</strong></span>"
           "</td>";
}
}  // namespace detail

inline std::string getHTMLTableRowString(const std::string& left, const std::string& right) {
    return "<tr>" + detail::getHTMLTableColumnString(left) +
           detail::getHTMLTableColumnString(right) + "</tr>";
}

inline std::string getHTMLTableRowString(const std::string& left, const dvec3& right) {
    return "<tr>" + detail::getHTMLTableColumnString(left) +
           detail::getHTMLTableColumnString(glm::to_string(right)) + "</tr>";
}

inline std::string getHTMLTableRowString(const std::string& left, const size3_t& right) {
    return "<tr>" + detail::getHTMLTableColumnString(left) +
           detail::getHTMLTableColumnString(glm::to_string(uvec3(right))) + "</tr>";
}

inline std::string getHTMLTableRowString(const std::string& left, const size_t right) {
    return "<tr>" + detail::getHTMLTableColumnString(left) +
           detail::getHTMLTableColumnString(std::to_string(right)) + "</tr>";
}

inline std::string getHTMLTableRowString(const std::string& left, const double right) {
    return "<tr>" + detail::getHTMLTableColumnString(left) +
           detail::getHTMLTableColumnString(std::to_string(right)) + "</tr>";
}

inline std::string getHTMLTableIntermediateHeaderString(const std::string& header,
                                                        size_t numberOfColumnsToSpan = 2) {
    return "<tr>" + detail::getHTMLTableColumnSpanString(header, numberOfColumnsToSpan) + "</tr>";
}

template <typename T>
T normalizeValue(const T& val, const T& minVal, const T& maxVal) {
    if (val >= maxVal) return T(1);
    if (val <= minVal) return T(0);
    return (val - minVal) / (maxVal - minVal);
}

template <typename T>
T denormalizeValue(const T& val, const T& minVal, const T& maxVal) {
    if (val <= T(0)) return minVal;
    if (val >= T(1)) return maxVal;
    return minVal + val * (maxVal - minVal);
}

template <typename T>
std::vector<T> divide(std::vector<T>& vec, T divisor) {
    std::vector<T> divisorVec;
    auto vector(vec);
    divisorVec.resize(vector.size(), divisor);
    std::transform(vector.begin(), vector.end(), divisorVec.begin(), vector.begin(),
                   std::divides<>());
    return vector;
}

template <typename T>
std::vector<T> operator/(const std::vector<T>& vec, T divisor) {
    std::vector<T> divisorVec;
    auto vector(vec);
    divisorVec.resize(vector.size(), divisor);
    std::transform(vector.begin(), vector.end(), divisorVec.begin(), vector.begin(),
                   std::divides<>());
    return vector;
}

template <typename T>
T nthRoot(T value, T n) {
    return std::pow(value, T(1) / n);
}

// Input: A collection of vectors
// Output: Intersection set of the entries of the input vectors
template <typename T>
std::vector<T> intersection(const std::vector<std::vector<T>>& vecs) {
    auto last_intersection = vecs[0];
    std::vector<T> curr_intersection;

    for (size_t i{1}; i < vecs.size(); ++i) {
        std::set_intersection(last_intersection.begin(), last_intersection.end(), vecs[i].begin(),
                              vecs[i].end(), std::back_inserter(curr_intersection));
        std::swap(last_intersection, curr_intersection);
        curr_intersection.clear();
    }
    return last_intersection;
}

// Input: A collection of vectors
// Output: Union set of the entries of the input vectors
template <typename T>
std::vector<T> union_set(const std::vector<std::vector<T>>& vecs) {
    auto last_union = vecs[0];
    std::vector<T> curr_union;

    for (size_t i{1}; i < vecs.size(); ++i) {
        std::set_union(last_union.begin(), last_union.end(), vecs[i].begin(), vecs[i].end(),
                       std::back_inserter(curr_union));
        std::swap(last_union, curr_union);
        curr_union.clear();
    }
    return last_union;
}

// Input: Two vectors
// Output: Intersection set of the entries of the input vectors
template <typename T>
std::vector<T> union_set(const std::vector<T>& vec1, const std::vector<T>& vec2) {
    std::vector<T> ret;
    std::set_union(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), std::back_inserter(ret));
    return ret;
}

template <typename Container, typename... Containers>
auto concatenate(Container first, Containers... vectorsToAppend) {
    (first.insert(std::end(first), std::begin(vectorsToAppend), std::end(vectorsToAppend)), ...);

    return std::move(first);
}

template <typename T>
std::vector<T> unique(const std::vector<T>& vector) {
    std::vector<T> ret(vector);
    auto last = std::unique(ret.begin(), ret.end());
    ret.erase(last, ret.end());
    return ret;
}

template <typename T>
std::string to_string(const T& list) {
    std::stringstream ss;
    ss << "[ ";

    if (list.empty()) {
        ss << ']';
        return ss.str();
    }

    for (const auto& val : list) {
        ss << std::to_string(val) << ", ";
    }

    auto str = ss.str();

    str[str.size() - 2] = ' ';
    str[str.size() - 1] = ']';

    return str;
}

template <typename T, size_t N>
std::string to_string(const std::array<T, N>& arr) {
    std::stringstream ss;
    ss << "[ ";

    for (size_t i{0}; i < N; ++i) {
        ss << std::to_string(arr[i]) << ", ";
    }

    auto str = ss.str();

    str[str.size() - 2] = ' ';
    str[str.size() - 1] = ']';

    return str;
}

inline vec3 hcl2rgb(const vec3& hcl) {
    const auto PI = 3.1415926536f;
    const auto hclGamma = 3.0f;
    const auto hclY0 = 100.0f;
    const auto hclMaxL = 0.530454533953517f;

    vec3 rgb = vec3(0.0f);

    if (hcl.z != 0.0f) {
        auto h = hcl.x;
        auto c = hcl.y;
        auto l = hcl.z * hclMaxL;
        auto q = glm::exp((1.0f - c / (2.0f * l)) * (hclGamma / hclY0));
        auto u = (2.0f * l - c) / (2.0f * q - 1.0f);
        auto v = c / q;
        auto t = glm::tan(
            (h + glm::min(glm::fract(2.0f * h) / 4.0f, glm::fract(-2.0f * h) / 8.0f)) * PI * 2.0f);
        h *= 6.0f;
        if (h <= 1.0f) {
            rgb.r = 1.0f;
            rgb.g = t / (1.0f + t);
        } else if (h <= 2.0f) {
            rgb.r = (1.0f + t) / t;
            rgb.g = 1.0f;
        } else if (h <= 3.0f) {
            rgb.g = 1.0f;
            rgb.b = 1.0f + t;
        } else if (h <= 4.0f) {
            rgb.g = 1.0f / (1.0f + t);
            rgb.b = 1.0f;
        } else if (h <= 5.0f) {
            rgb.r = -1.0f / t;
            rgb.b = 1.0f;
        } else {
            rgb.r = 1.0f;
            rgb.b = -t;
        }
        rgb = rgb * v + u;
    }

    return rgb;
}

inline vec3 rgb2hcl(const vec3& rgb) {
    const float PI = 3.1415926536f;
    const float hclGamma = 3.0f;
    const float hclY0 = 100.0f;
    const float hclMaxL = 0.530454533953517f;

    vec3 hcl;
    float h = 0.0f;
    float u = glm::min(rgb.r, glm::min(rgb.g, rgb.b));
    float v = glm::max(rgb.r, glm::max(rgb.g, rgb.b));
    auto q = hclGamma / hclY0;
    hcl.y = v - u;

    if (hcl.y != 0.0f) {
        h = glm::atan(rgb.g - rgb.b, rgb.r - rgb.g) / PI;
        q *= u / v;
    }

    q = exp(q);
    hcl.x = glm::fract(h / 2.0f - glm::min(glm::fract(h), glm::fract(-h)) / 6.0f);
    hcl.y *= q;
    hcl.z = glm::mix(-u, v, q) / (hclMaxL * 2.0f);

    return hcl;
}

// Get quadratic Bezier Spline Control Point.
inline vec2 getQuadraticBezierCurveControlPoint(const vec2& p0, const vec2& p1, const vec2& p2,
                                                const float t = 0.5f) {
    return t * t * p0 + t * p1 + t * t * p2;
}

// Get quadratic Bezier Spline Control Points.
inline std::vector<vec2> getQuadraticBezierCurveControlPoints(
    const std::vector<vec2>& startAndEndPoints, const std::vector<vec2>& intermediatePoints) {
    std::vector<vec2> controlPoints{};

    const auto n = startAndEndPoints.size();

    if (n < 1) {
        LogErrorCustom("Method getQuadraticBezierCurveControlPoints tensorvisbase/util/misc.h",
                       "At least two knot points required.");
        return controlPoints;
    }

    const auto m = intermediatePoints.size();

    if (m != n - 1) {
        LogErrorCustom("Method getQuadraticBezierCurveControlPoints tensorvisbase/util/misc.h",
                       "Requirement n-1 == m not satisfied.");
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

// Solves a tridiagonal system for one of coordinates (x or y) of first Bezier control points.
inline std::vector<float> GetFirstControlPoints(const std::vector<float>& rhs) {
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

// Get cubic Bezier Spline Control Points.
// https://www.codeproject.com/Articles/31859/Draw-a-Smooth-Curve-through-a-Set-of-2D-Points-wit
inline std::pair<std::vector<vec2>, std::vector<vec2>> getCubicBezierCurveControlPoints(
    const std::vector<vec2>& knots) {
    std::pair<std::vector<vec2>, std::vector<vec2>> controlPoints{};
    auto& firstControlPoints = controlPoints.first;
    auto& secondControlPoints = controlPoints.second;

    const auto n = knots.size() - 1;

    if (n < 1) {
        LogErrorCustom("Method getCubicBezierCurveControlPoints in tensorvisbase/util/misc.h",
                       "At least two knot points required");
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

}  // namespace tensorutil
}  // namespace inviwo