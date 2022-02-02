/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2021 Inviwo Foundation
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

#include <inviwo/ttk/processors/ttkgenericprocessor.h>

#include "vtkType.h"

namespace inviwo {

namespace vtk {

// Copied from vtkDataObjectTypes in version 9.0.2
namespace {
bool IsTypeIdValid(int typeId) {
    return (typeId >= VTK_POLY_DATA && typeId <= VTK_EXPLICIT_STRUCTURED_GRID);
}
}  // namespace

int GetCommonBaseTypeId(int typeA, int typeB) {
    if (!IsTypeIdValid(typeA) && !IsTypeIdValid(typeB)) {
        return -1;
    } else if (!IsTypeIdValid(typeA) && IsTypeIdValid(typeB)) {
        return typeB;
    } else if (IsTypeIdValid(typeA) && !IsTypeIdValid(typeB)) {
        return typeA;
    }

    auto computeBranch = [](int type) {
        // list immediate base-classes, no need to list any that are direct subclasses
        // of vtkDataObject since that's assumed by this point.
        static const std::map<int, int> bases = {
            {VTK_UNIFORM_HYPER_TREE_GRID, VTK_HYPER_TREE_GRID},
            {VTK_UNDIRECTED_GRAPH, VTK_GRAPH},
            {VTK_DIRECTED_GRAPH, VTK_GRAPH},
            {VTK_MOLECULE, VTK_UNDIRECTED_GRAPH},
            {VTK_DIRECTED_ACYCLIC_GRAPH, VTK_DIRECTED_GRAPH},
            {VTK_REEB_GRAPH, VTK_DIRECTED_GRAPH},
            {VTK_TREE, VTK_DIRECTED_ACYCLIC_GRAPH},
            {VTK_RECTILINEAR_GRID, VTK_DATA_SET},
            {VTK_POINT_SET, VTK_DATA_SET},
            {VTK_IMAGE_DATA, VTK_DATA_SET},
            {VTK_UNSTRUCTURED_GRID_BASE, VTK_POINT_SET},
            {VTK_STRUCTURED_GRID, VTK_POINT_SET},
            {VTK_POLY_DATA, VTK_POINT_SET},
            {VTK_PATH, VTK_POINT_SET},
            {VTK_EXPLICIT_STRUCTURED_GRID, VTK_POINT_SET},
            {VTK_UNSTRUCTURED_GRID, VTK_UNSTRUCTURED_GRID_BASE},
            {VTK_UNIFORM_GRID, VTK_IMAGE_DATA},
            {VTK_STRUCTURED_POINTS, VTK_IMAGE_DATA},
            {VTK_OVERLAPPING_AMR, VTK_UNIFORM_GRID_AMR},
            {VTK_HIERARCHICAL_BOX_DATA_SET, VTK_OVERLAPPING_AMR},
            {VTK_NON_OVERLAPPING_AMR, VTK_UNIFORM_GRID_AMR},
            {VTK_PARTITIONED_DATA_SET_COLLECTION, VTK_COMPOSITE_DATA_SET},
            {VTK_PARTITIONED_DATA_SET, VTK_COMPOSITE_DATA_SET},
            {VTK_MULTIPIECE_DATA_SET, VTK_PARTITIONED_DATA_SET},
            {VTK_MULTIBLOCK_DATA_SET, VTK_COMPOSITE_DATA_SET}};

        std::vector<int> branch;
        do {
            branch.push_back(type);
            auto iter = bases.find(type);
            type = (iter != bases.end()) ? iter->second : VTK_DATA_OBJECT;
        } while (type != VTK_DATA_OBJECT);
        branch.push_back(VTK_DATA_OBJECT);
        std::reverse(branch.begin(), branch.end());
        return branch;
    };

    std::vector<int> branchA = computeBranch(typeA);
    std::vector<int> branchB = computeBranch(typeB);

    int baseType = VTK_DATA_OBJECT;
    size_t index = 0;
    for (size_t max = std::min(branchA.size(), branchB.size()); index < max; ++index) {
        if (branchA[index] == branchB[index]) {
            baseType = branchA[index];
        } else {
            break;
        }
    }
    return baseType;
}

bool TypeIdIsA(int typeId, int targetTypeId) {
    if (!IsTypeIdValid(typeId) || !IsTypeIdValid(targetTypeId)) {
        return false;
    }

    if (typeId == targetTypeId) {
        return true;
    }

    if (GetCommonBaseTypeId(typeId, targetTypeId) == targetTypeId) {
        return true;
    }

    return false;
}

// End from vtkDataObjectTypes

void addTable(Document::DocumentHandle& h, std::string_view str) {

    auto r = h.append("table").append("tr");
    size_t curr = 0;
    size_t offset = 0;
    size_t count = 0;

    while (offset != std::string_view::npos) {
        while (offset != std::string_view::npos && count < 35) {
            offset = str.find('\n', offset+1);
            ++count;
        }
        r.append("td").append("pre", str.substr(curr, offset - curr));
        curr = offset;
        count = 0;
    }
}

}  // namespace vtk

}  // namespace inviwo
