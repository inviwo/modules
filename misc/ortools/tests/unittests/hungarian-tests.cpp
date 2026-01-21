/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2026 Inviwo Foundation
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

#include <warn/push>
#include <warn/ignore/all>
#include <gtest/gtest.h>
#include <warn/pop>

#include <ortools/algorithms/hungarian.h>
#include <ortools/base/map_util.h>
#include "absl/container/flat_hash_map.h"

namespace inviwo {

namespace {

void GenericCheck(const int expected_assignment_size,
                  const absl::flat_hash_map<int, int>& direct_assignment,
                  const absl::flat_hash_map<int, int>& reverse_assignment,
                  const std::array<int, 4>& expected_agents, const std::array<int, 4>& expected_tasks) {
    EXPECT_EQ(expected_assignment_size, direct_assignment.size());
    EXPECT_EQ(expected_assignment_size, reverse_assignment.size());
    for (int i = 0; i < expected_assignment_size; ++i) {
        EXPECT_EQ(gtl::FindOrDie(direct_assignment, expected_agents[i]), expected_tasks[i]);
        EXPECT_EQ(gtl::FindOrDie(reverse_assignment, expected_tasks[i]), expected_agents[i]);
    }
    for (const auto& direct_iter : direct_assignment) {
        EXPECT_EQ(gtl::FindOrDie(reverse_assignment, direct_iter.second), direct_iter.first)
            << direct_iter.first << " -> " << direct_iter.second;
    }
}

void TestMinimization(absl::Span<const std::vector<double>> cost,
                      const int expected_assignment_size, const std::array<int, 4>& expected_agents,
                      const std::array<int, 4>& expected_tasks) {
    absl::flat_hash_map<int, int> direct_assignment;
    absl::flat_hash_map<int, int> reverse_assignment;
    operations_research::MinimizeLinearAssignment(cost, &direct_assignment, &reverse_assignment);

    SCOPED_TRACE("Minimization");
    GenericCheck(expected_assignment_size, direct_assignment, reverse_assignment, expected_agents,
                 expected_tasks);
}

void TestMaximization(absl::Span<const std::vector<double>> cost,
                      const int expected_assignment_size, const std::array<int, 4>& expected_agents,
                      const std::array<int, 4>& expected_tasks) {
    absl::flat_hash_map<int, int> direct_assignment;
    absl::flat_hash_map<int, int> reverse_assignment;
    operations_research::MaximizeLinearAssignment(cost, &direct_assignment, &reverse_assignment);
    SCOPED_TRACE("Maximization");
    GenericCheck(expected_assignment_size, direct_assignment, reverse_assignment, expected_agents,
                 expected_tasks);
}

}  // namespace

TEST(ortools, hungarianTests) {

    const int kMatrixHeight = 4;
    const int kMatrixWidth = 4;
    const double kCost[kMatrixHeight][kMatrixWidth] = {
        {90, 75, 75, 80}, {35, 85, 55, 65}, {125, 95, 90, 105}, {45, 110, 95, 115}};

    constexpr std::array<int, 4> expected_agents_for_min = {0, 1, 2, 3};
    constexpr std::array<int, 4> expected_tasks_for_min = {3, 2, 1, 0};
    constexpr std::array<int, 4> expected_agents_for_max = {0, 1, 2, 3};
    constexpr std::array<int, 4> expected_tasks_for_max = {2, 1, 0, 3};

    std::vector<std::vector<double>> cost(kMatrixHeight);
    for (int row = 0; row < kMatrixHeight; ++row) {
        cost[row].resize(kMatrixWidth);
        for (int col = 0; col < kMatrixWidth; ++col) {
            cost[row][col] = kCost[row][col];
        }
    }

    const int assignment_size = expected_agents_for_max.size();
    TestMinimization(cost, assignment_size, expected_agents_for_min, expected_tasks_for_min);
    TestMaximization(cost, assignment_size, expected_agents_for_max, expected_tasks_for_max);
}

}  // namespace inviwo
