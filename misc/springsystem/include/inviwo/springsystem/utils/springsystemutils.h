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

#include <inviwo/springsystem/springsystemmoduledefine.h>
#include <inviwo/springsystem/datastructures/springsystem.h>
#include <inviwo/core/common/inviwo.h>

namespace inviwo {

namespace springmass {

template <size_t N, typename ComponentType = double>
struct Grid {
    std::vector<glm::vec<N, ComponentType>> positions;
    std::vector<std::pair<std::size_t, std::size_t>> springs;
    std::vector<bool> locked;
};

template <size_t N, typename ComponentType = double>
auto createLineGrid(std::size_t numNodes, ComponentType deltaDist) -> Grid<N, ComponentType>;

template <size_t N, typename ComponentType = double>
auto createRectangularGrid(size2_t gridDim, glm::vec<N, ComponentType> origin,
                           glm::vec<N, ComponentType> deltaDist) -> Grid<N, ComponentType>;

template <size_t N, typename ComponentType = double>
auto createDiagonalGridDiagonal(size2_t gridDim, glm::vec<N, ComponentType> origin,
                                glm::vec<N, ComponentType> deltaDist) -> Grid<N, ComponentType>;

template <size_t N, typename ComponentType = double>
auto createHexagonalGrid(size2_t gridDim, glm::vec<N, ComponentType> origin,
                         glm::vec<N, ComponentType> deltaDist) -> Grid<N, ComponentType>;

// implementations

template <size_t N, typename ComponentType>
auto createLineGrid(std::size_t numNodes, ComponentType deltaDist) -> Grid<N, ComponentType> {
    using Vector = glm::vec<N, ComponentType>;
    static_assert(N >= 1, "");

    if (numNodes < 2) numNodes = 2;

    Grid<2, double> res{std::vector<Vector>(numNodes, Vector{0}),
                        std::vector<std::pair<std::size_t, std::size_t>>(numNodes - 1),
                        std::vector<bool>(numNodes, false)};

    // add nodes placed equidistantly
    const ComponentType origin((numNodes - 1) * deltaDist * 0.5);
    for (std::size_t i = 0; i < numNodes; ++i) {
        res.positions[i].x = -origin + deltaDist * i;
    }
    // add springs in between nodes
    for (std::size_t i = 0; i < numNodes - 1; ++i) {
        res.springs[i] = {i, i + 1};
    }

    // mark first node as anchor point
    res.locked[0] = true;
    return res;
}

template <size_t N, typename ComponentType>
auto createRectangularGrid(size2_t gridDim, glm::vec<N, ComponentType> origin,
                           glm::vec<N, ComponentType> deltaDist) -> Grid<N, ComponentType> {

    static_assert(N >= 2, "");
    using Vector = glm::vec<N, ComponentType>;

    gridDim = glm::max(size2_t(2, 2), gridDim);
    const std::size_t numNodes = gridDim.x * gridDim.y;
    const std::size_t numSprings = (gridDim.x - 1) * gridDim.y + gridDim.x * (gridDim.y - 1);

    Grid<2, double> res{std::vector<Vector>(numNodes, Vector{0}),
                        std::vector<std::pair<std::size_t, std::size_t>>(numSprings),
                        std::vector<bool>(numNodes, false)};

    // add nodes placed equidistantly
    for (std::size_t j = 0; j < gridDim.y; ++j) {
        for (std::size_t i = 0; i < gridDim.x; ++i) {
            Vector pos{};
            pos.x = deltaDist.x * i;
            pos.y = -deltaDist.y * j;
            res.positions[i + gridDim.x * j] = origin + pos;
        }
    }

    // add springs in between nodes
    // horizontal springs
    size_t count = 0;
    for (std::size_t j = 0; j < gridDim.y; ++j) {
        for (std::size_t i = 0; i < gridDim.x - 1; ++i) {
            auto rowIndex = j * gridDim.x;
            res.springs[count] = {i + rowIndex, i + 1 + rowIndex};
            ++count;
        }
    }
    // vertical springs
    for (std::size_t i = 0; i < gridDim.x; ++i) {
        for (std::size_t j = 0; j < gridDim.y - 1; ++j) {
            res.springs[count] = {i + j * gridDim.x, i + (j + 1) * gridDim.x};
            ++count;
        }
    }

    // mark first and last node of top line as anchor
    res.locked[0] = true;
    res.locked[gridDim.x - 1] = true;

    return res;
}

template <size_t N, typename ComponentType>
auto createDiagonalGridDiagonal(size2_t gridDim, glm::vec<N, ComponentType> origin,
                                glm::vec<N, ComponentType> deltaDist) -> Grid<N, ComponentType> {

    static_assert(N >= 2, "");
    using Vector = glm::vec<N, ComponentType>;

    gridDim = glm::max(size2_t(2, 2), gridDim);
    const std::size_t numNodes = gridDim.x * gridDim.y;
    const std::size_t numSprings = (gridDim.x - 1) * gridDim.y + gridDim.x * (gridDim.y - 1) +
                                   2 * (gridDim.x - 1) * (gridDim.y - 1);

    Grid<2, double> res{std::vector<Vector>(numNodes, Vector{0}),
                        std::vector<std::pair<std::size_t, std::size_t>>(numSprings),
                        std::vector<bool>(numNodes, false)};

    // add nodes placed equidistantly
    for (std::size_t j = 0; j < gridDim.y; ++j) {
        for (std::size_t i = 0; i < gridDim.x; ++i) {
            Vector pos{};
            pos.x = deltaDist.x * i;
            pos.y = -deltaDist.y * j;
            res.positions[i + gridDim.x * j] = origin + pos;
        }
    }

    // add springs in between nodes
    // horizontal springs
    for (std::size_t j = 0; j < gridDim.y; ++j) {
        for (std::size_t i = 0; i < gridDim.x - 1; ++i) {
            auto rowIndex = j * gridDim.x;
            res.springs.push_back({i + rowIndex, i + 1 + rowIndex});
        }
    }
    // vertical springs
    size_t count = 0;
    for (std::size_t i = 0; i < gridDim.x; ++i) {
        for (std::size_t j = 0; j < gridDim.y - 1; ++j) {
            res.springs[count] = {i + j * gridDim.x, i + (j + 1) * gridDim.x};
            ++count;
        }
    }
    // add diagonals
    for (std::size_t j = 0; j < gridDim.y - 1; ++j) {
        for (std::size_t i = 0; i < gridDim.x - 1; ++i) {
            res.springs[count] = {i + j * gridDim.x, i + 1 + (j + 1) * gridDim.x};
            ++count;
            res.springs[count] = {i + (j + 1) * gridDim.x, i + 1 + j * gridDim.x};
            ++count;
        }
    }

    // mark first and last node of top line as anchor
    res.locked[0] = true;
    res.locked[gridDim.x - 1] = true;

    return res;
}

template <size_t N, typename ComponentType>
auto createHexagonalGrid(size2_t gridDim, glm::vec<N, ComponentType> origin,
                         glm::vec<N, ComponentType> deltaDist) -> Grid<N, ComponentType> {

    static_assert(N >= 2, "");
    using Vector = glm::vec<N, ComponentType>;

    gridDim = glm::max(size2_t(2, 3), gridDim);
    if ((gridDim.y & 1) == 0) {
        // make sure, y dimension is odd
        gridDim.y += 1;
    }

    const std::size_t nodesPerLine = gridDim.x + 1;
    const std::size_t nodesPerCol = gridDim.y + 1;

    const std::size_t numNodes = (2 * (nodesPerCol - 1)) * (nodesPerLine - 1) + nodesPerCol;
    const std::size_t numBorderSprings = 2 * gridDim.x + 2 * (gridDim.y / 2);
    const std::size_t numSprings = nodesPerCol / 2 * nodesPerLine +
                                   5 * (gridDim.y / 2) * (nodesPerLine - 1) + numBorderSprings;

    using Vec = glm::vec<2, ComponentType>;
    Grid<2, double> res{std::vector<Vector>(), std::vector<std::pair<std::size_t, std::size_t>>(),
                        std::vector<bool>(numNodes, false)};

    res.positions.reserve(numNodes);
    res.springs.reserve(numSprings);

    // hexagon defined by width and its height = w * 2 / sqrt(3)
    const auto width = deltaDist.x;
    const auto height = 2.0f / glm::sqrt(3.0f) * width;

    // first line - nodes are 2*w apart
    for (std::size_t i = 0; i < nodesPerLine; ++i) {
        res.positions.push_back(origin + Vec(width * i, 0));
        if (i > 0) {
            res.springs.push_back({i - 1, i});
        }
    }
    // loop over 0 < i < 4*(m - 1), four cases
    std::size_t line = 1;
    ComponentType verticalLinePos = 0.0f;
    for (std::size_t l = 1; l < (nodesPerCol - 1) * 2; ++l, ++line) {
        const auto lineIndex = res.positions.size();
        if (line == 1) {
            // first case, n nodes, connected vertically down
            verticalLinePos -= static_cast<ComponentType>(0.5) * height;
            for (std::size_t i = 0; i < nodesPerLine; ++i) {
                res.positions.push_back(origin + Vec(width * i, verticalLinePos));
                res.springs.push_back({lineIndex - nodesPerLine + i, lineIndex + i});
            }
        } else if (line == 2) {
            verticalLinePos -= static_cast<ComponentType>(0.25) * height;
            for (std::size_t i = 0; i < nodesPerLine - 1; ++i) {
                res.positions.push_back(origin + Vec(width * i + 0.5f * width, verticalLinePos));
                res.springs.push_back({lineIndex + i, lineIndex - nodesPerLine + i});
                res.springs.push_back({lineIndex + i, lineIndex - nodesPerLine + i + 1});
            }
        } else if (line == 3) {
            verticalLinePos -= static_cast<ComponentType>(0.5) * height;
            for (std::size_t i = 0; i < nodesPerLine - 1; ++i) {
                res.positions.push_back(origin + Vec(width * i + 0.5f * width, verticalLinePos));
                res.springs.push_back({lineIndex - (nodesPerLine - 1) + i, lineIndex + i});
            }
        } else {
            // fourth case
            verticalLinePos -= static_cast<ComponentType>(0.25) * height;
            for (std::size_t i = 0; i < nodesPerLine; ++i) {
                res.positions.push_back(origin + Vec(width * i, verticalLinePos));
                if (i == 0) {
                    // spring at the outer-left edge
                    res.springs.push_back({lineIndex, lineIndex - 3 * nodesPerLine + 2});
                }
                if (i + 1 == nodesPerLine) {
                    // spring at the outer-right edge
                    res.springs.push_back({lineIndex + i, lineIndex - 3 * nodesPerLine + 2 + i});
                }
                if (i > 0) {
                    // spring to lower left neighbor
                    res.springs.push_back({lineIndex + i, lineIndex - nodesPerLine + i});
                }
                if (i + 1 < nodesPerLine) {
                    // spring to lower right neighbor
                    res.springs.push_back({lineIndex + i, lineIndex - (nodesPerLine - 1) + i});
                }
            }
            line -= 4;
        }
    }
    // springs last line
    for (std::size_t i = 1; i < nodesPerLine; ++i) {
        res.springs.push_back({numNodes - nodesPerLine + i - 1, numNodes - nodesPerLine + i});
    }

    ivwAssert(res.positions.size() == numNodes, "number of positions not equal to numNodes");
    ivwAssert(res.springs.size() == numSprings, "number of springs mismatch");

    // mark first and last node as anchor
    res.locked[0] = true;
    res.locked[nodesPerLine - 1] = true;

    return res;
}

}  // namespace springmass

}  // namespace inviwo
