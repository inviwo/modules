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

#include <inviwo/ortools/processors/hungarian.h>

#include <ortools/algorithms/hungarian.h>
#include "absl/container/flat_hash_map.h"

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo Hungarian::processorInfo_{
    "org.inviwo.Hungarian",   // Class identifier
    "Hungarian",              // Display name
    "Undefined",              // Category
    CodeState::Experimental,  // Code state
    Tags::None,               // Tags
    R"(<Explanation of how to use the processor.>)"_unindentHelp,
};

const ProcessorInfo& Hungarian::getProcessorInfo() const { return processorInfo_; }

Hungarian::Hungarian()
    : Processor{}
    , size_{"size",
            "Size",
            "<description of the property>"_help,
            vec3(0.0f),
            {vec3(-100.0f), ConstraintBehavior::Ignore},
            {vec3(100.0f), ConstraintBehavior::Ignore}} {

    addProperties(size_);
}

void Hungarian::process() {
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

    absl::flat_hash_map<int, int> direct_assignment;
    absl::flat_hash_map<int, int> reverse_assignment;
    operations_research::MinimizeLinearAssignment(cost, &direct_assignment, &reverse_assignment);

    for (auto& [key, value] : direct_assignment) {
        log::info("assign {} -> {}", key, value);
    }
}

}  // namespace inviwo
