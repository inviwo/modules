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

#include <inviwo/integrallinefiltering/integrallinefilteringmoduledefine.h>
#include <inviwo/core/util/glm.h>
#include <inviwo/core/util/foreach.h>

#include <inviwo/integrallinefiltering/utils/sparsehistogram.h>

namespace inviwo {
namespace detail {}

template <unsigned Dims = 3, typename T = double>
class DirectionalHistogram {
    static_assert(std::is_floating_point_v<T>);

    using type = glm::vec<Dims, T>;

private:
    DirectionalHistogram(std::vector<type> directions)
        : directions_(std::move(directions)), bins_(directions_.size(), 0) {}

public:
    DirectionalHistogram(const DirectionalHistogram &) = default;
    DirectionalHistogram(DirectionalHistogram &&) = default;
    DirectionalHistogram &operator=(const DirectionalHistogram &) = default;
    DirectionalHistogram &operator=(DirectionalHistogram &&) = default;

    auto begin() { return bins_.begin(); }
    auto end() { return bins_.end(); }
    auto begin() const { return bins_.begin(); }
    auto end() const { return bins_.end(); }

    size_t numberOfBins() const { return bins_.size(); }

    virtual ~DirectionalHistogram() = default;

    void inc(const type &dir);

    static DirectionalHistogram createFromCircle(size_t directions = 0);
    static DirectionalHistogram createFromIcosahedron(size_t subdivision = 0);

private:
    std::vector<type> directions_;
    std::vector<size_t> bins_;
};

template <unsigned Dims, typename T>
void DirectionalHistogram<Dims, T>::inc(const type &dir) {
    T max = std::numeric_limits<T>::lowest();
    size_t maxindex;
    util::forEach(directions_, [&](const auto &d, size_t i) {
        const auto dot = glm::dot(d, dir);
        if (max < dot) {
            maxindex = i;
            max = dot;
        }
    });
    IVW_ASSERT(maxindex < bins_.size(),
               "maxindex should not be able to largers than number of bins");
    bins_[maxindex]++;
}

template <unsigned Dims, typename T>
DirectionalHistogram<Dims, T> DirectionalHistogram<Dims, T>::createFromCircle(
    size_t numDirections) {
    static_assert(Dims == 2, "createFromCircle only works in 2 dimenssions");

    std::vector<type> directions;
    const T da = glm::two_pi<T>() / static_cast<T>(numDirections);
    for (size_t i = 0; i < numDirections; i++) {
        float angle = i * da;
        directions.emplace_back(std::cos(angle), std::sin(angle));
    }
    return {directions};
}

template <unsigned Dims, typename T>
DirectionalHistogram<Dims, T> DirectionalHistogram<Dims, T>::createFromIcosahedron(
    size_t subdivision) {
    static_assert(Dims == 3, "createFromIcosahedron only works in 3 dimenssions");

    static const T alpha = static_cast<T>(0.525731087);
    static const T beta = static_cast<T>(0.850650787);
    static const T zero = 0;

    static const std::array<type, 12> startVertices = {{{-alpha, zero, beta},
                                                        {alpha, zero, beta},
                                                        {-alpha, zero, -beta},
                                                        {alpha, zero, -beta},
                                                        {zero, beta, alpha},
                                                        {zero, beta, -alpha},
                                                        {zero, -beta, alpha},
                                                        {zero, -beta, -alpha},
                                                        {beta, alpha, zero},
                                                        {-beta, alpha, zero},
                                                        {beta, -alpha, zero},
                                                        {-beta, -alpha, zero}}};

    constexpr static const std::array<size3_t, 20> startTriangles = {
        {{0, 4, 1}, {0, 9, 4},  {9, 5, 4},  {4, 5, 8},  {4, 8, 1},  {8, 10, 1}, {8, 3, 10},
         {5, 3, 8}, {5, 2, 3},  {2, 7, 3},  {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6},
         {0, 1, 6}, {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5},  {7, 2, 11}}};

    std::vector<type> vertices(startVertices.begin(), startVertices.end());
    std::vector<size3_t> triangles(startTriangles.begin(), startTriangles.end());

    for (size_t i = 0; i < subdivision; i++) {
        std::vector<size3_t> newTriangles;
        for (const auto &t : triangles) {
            auto i0 = vertices.size() + 0;
            auto i1 = vertices.size() + 1;
            auto i2 = vertices.size() + 2;
            vertices.push_back(glm::normalize(vertices[t.x] + vertices[t.y]));
            vertices.push_back(glm::normalize(vertices[t.y] + vertices[t.z]));
            vertices.push_back(glm::normalize(vertices[t.z] + vertices[t.x]));

            newTriangles.emplace_back(t.x, i0, i2);
            newTriangles.emplace_back(i0, t.y, i1);
            newTriangles.emplace_back(i0, i1, i2);
            newTriangles.emplace_back(i2, i1, t.z);
        }
        swap(triangles, newTriangles);
    }

    std::vector<type> directions;
    for (const auto &t : triangles) {
        const auto &v0 = vertices[t.x];
        const auto &v1 = vertices[t.y];
        const auto &v2 = vertices[t.z];
        directions.push_back(glm::normalize(v0 + v1 + v2));
    }

    return {directions};
}

}  // namespace inviwo
