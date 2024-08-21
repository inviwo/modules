/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2023-2024 Inviwo Foundation
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

#include <inviwo/vtk/datastructures/vtktetramesh.h>
#include <inviwo/tetramesh/util/tetrameshutils.h>

#include <inviwo/core/util/exception.h>
#include <inviwo/core/util/zip.h>
#include <inviwo/core/util/logcentral.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/glm.h>

#include <vtkUnstructuredGrid.h>
#include <vtkInformation.h>
#include <vtkType.h>
#include <vtkSmartPointer.h>
#include <vtkCellIterator.h>

#include <algorithm>

namespace inviwo {

namespace detail {

void getNodes(vtkUnstructuredGrid* vtkData, const std::shared_ptr<BufferBase>& scalarBuffer,
              std::vector<vec4>& nodes) {
    const vtkIdType numPoints = vtkData->GetNumberOfPoints();

    nodes.reserve(numPoints);
    for (vtkIdType pointId = 0; pointId < numPoints; ++pointId) {
        auto* point = vtkData->GetPoint(pointId);
        nodes.emplace_back(point[0], point[1], point[2], 0.0f);
    }

    // store scalar values in the 4th component of the nodes
    if (scalarBuffer) {
        if (scalarBuffer->getSize() != nodes.size()) {
            throw Exception(IVW_CONTEXT_CUSTOM("VTKTetraMesh"),
                            "Size of scalar buffer ({}) doese not match the number of nodes ({}).",
                            scalarBuffer->getSize(), nodes.size());
        }

        scalarBuffer->getRepresentation<BufferRAM>()
            ->template dispatch<void, dispatching::filter::Scalars>([&](auto br) {
                const auto& data = br->getDataContainer();
                for (auto&& elem : util::zip(nodes, data)) {
                    elem.first().w = static_cast<float>(elem.second());
                }
            });
    }
}

void getNodeIds(vtkUnstructuredGrid* vtkData, std::vector<ivec4>& nodeIds) {
    const vtkIdType numCells = vtkData->GetNumberOfCells();

    nodeIds.reserve(numCells);

    int skippedCells = 0;
    vtkSmartPointer<vtkCellIterator> it = vtkData->NewCellIterator();
    for (it->InitTraversal(); !it->IsDoneWithTraversal(); it->GoToNextCell()) {
        if (it->GetCellType() != VTK_TETRA) {
            ++skippedCells;
            continue;
        }
        auto ids = util::make_array<4>([it = it->GetPointIds()->begin()](auto) mutable {
            return static_cast<std::uint32_t>(*it++);
        });
        // store the apex of each triangle face
        nodeIds.emplace_back(ids[0], ids[1], ids[2], ids[3]);
    }
    if (skippedCells > 0) {
        util::logWarn(IVW_CONTEXT_CUSTOM("VTKTetraMesh"),
                      "Only tetrahedral cells are supported. Skipped {} of {} cells", skippedCells,
                      numCells);
    }
}

}  // namespace detail

VTKTetraMesh::VTKTetraMesh(utilvtk::ArrayBufferMapper& bufferMapper, vtkUnstructuredGrid* vtkData)
    : axes{util::defaultAxes<3>()}, bufferMapper_{bufferMapper}, vtkData_{nullptr} {
    setData(vtkData);
}

VTKTetraMesh* VTKTetraMesh::clone() const { return new VTKTetraMesh{*this}; }

void VTKTetraMesh::setData(vtkUnstructuredGrid* vtkData) {
    vtkData_ = vtkData;
    setModelMatrix(mat4(1.0f));
    setWorldMatrix(mat4(1.0f));
}

int VTKTetraMesh::getNumberOfCells() const {
    if (vtkData_) {
        return static_cast<int>(vtkData_->GetNumberOfCells());
    } else {
        return 0;
    }
}

int VTKTetraMesh::getNumberOfPoints() const {
    if (vtkData_) {
        return static_cast<int>(vtkData_->GetNumberOfPoints());
    } else {
        return 0;
    }
}

void VTKTetraMesh::get(std::vector<vec4>& nodes, std::vector<ivec4>& nodeIds) const {
    nodes.clear();
    nodeIds.clear();

    if (!vtkData_) {
        return;
    }

    auto scalarBuffer = [&]() -> std::shared_ptr<BufferBase> {
        auto buffers = bufferMapper_.getBuffers(vtkData_);
        auto it = std::find_if(buffers.begin(), buffers.end(),
                               [](auto p) { return p.first == BufferType::ScalarMetaAttrib; });
        if (it != buffers.end()) {
            return it->second;
        } else {
            return nullptr;
        }
    }();
    detail::getNodes(vtkData_, scalarBuffer, nodes);
    detail::getNodeIds(vtkData_, nodeIds);

    utiltetra::fixFaceOrientation(nodes, nodeIds);
}

mat4 VTKTetraMesh::getBoundingBox() const {
    vec3 dataMin{0.0f};
    vec3 dataMax{0.0f};
    if (vtkData_) {
        vtkData_->ComputeBounds();
        auto bounds = vtkData_->GetBounds();
        dataMin = vec3{bounds[0], bounds[2], bounds[4]};
        dataMax = vec3{bounds[1], bounds[3], bounds[5]};
    }

    auto m = glm::scale(dataMax - dataMin);
    m[3] = vec4(dataMin, 1.0f);
    return m;
}

dvec2 VTKTetraMesh::getDataRange() const {
    if (auto b = bufferMapper_.getBufferInfo(utilvtk::ArrayUsage::Scalar); b) {
        return b->getDataRange();
    } else {
        return dvec2{0.0, 1.0};
    }
}

const Axis* VTKTetraMesh::getAxis(size_t index) const {
    if (index >= 3) {
        return nullptr;
    }
    return &axes[index];
}

}  // namespace inviwo
