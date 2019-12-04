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

inline dvec3 vtk_ExtentFromBounds(const double* bounds) {
    return dvec3{bounds[1] - bounds[0], bounds[3] - bounds[2], bounds[5] - bounds[4]};
}

inline dvec3 vtk_OffsetFromBounds(const double* bounds) {
    return dvec3{bounds[0], bounds[2], bounds[4]};
}

}  // namespace tensorutil
}  // namespace inviwo