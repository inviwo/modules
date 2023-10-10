/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2023 Inviwo Foundation
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

#include <inviwo/ttk/processors/vtktomesh.h>

#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/core/util/utilities.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/interaction/events/pickingevent.h>

#include <inviwo/core/util/glm.h>

#include <vtkType.h>

#include <vtkDataObject.h>
#include <vtkDataSet.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkAbstractArray.h>
#include <vtkArrayDispatch.h>
#include <vtkDataArrayMeta.h>
#include <vtkCellIterator.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKToMesh::processorInfo_{
    "org.inviwo.VTKToMesh",               // Class identifier
    "VTK To Mesh",                        // Display name
    "VTK",                                // Category
    CodeState::Experimental,              // Code state
    Tag::CPU | Tag{"VTK"} | Tag{"Mesh"},  // Tags
    R"(Convert VTK DataSet to a inviwo Mesh)"_unindentHelp};

const ProcessorInfo VTKToMesh::getProcessorInfo() const { return processorInfo_; }

VTKToMesh::VTKToMesh()
    : Processor{}
    , inport_{"inport_", VTK_DATA_SET, ""_help}
    , brushLinkPort_{"brushingAndLinking"}
    , outport_{"outport_", ""_help}
    , bufferMapper_{*this, [this](PickingEvent* event) { picking(event); }} {

    addPorts(inport_, brushLinkPort_, outport_);
}

constexpr std::array<Mesh::MeshInfo, VTK_NUMBER_OF_CELL_TYPES> cellMap = []() {
    std::array<Mesh::MeshInfo, VTK_NUMBER_OF_CELL_TYPES> tmp;
    for (auto& item : tmp) {
        item.dt = DrawType::NotSpecified;
    }

    tmp[VTK_VERTEX].dt = DrawType::Points;
    tmp[VTK_POLY_VERTEX].dt = DrawType::Points;
    tmp[VTK_LINE].dt = DrawType::Lines;
    tmp[VTK_TRIANGLE].dt = DrawType::Triangles;
    tmp[VTK_TETRA].dt = DrawType::Triangles;

    return tmp;
}();

void VTKToMesh::process() {
    auto data = vtkDataSet::SafeDownCast(inport_.getData());

    if (inport_.isChanged()) {
        bufferMapper_.updateSources(data);
    }

    bool update = inport_.isChanged() || bufferMapper_.isModified();
    if (!update) return;

    auto mesh = std::make_shared<Mesh>(bufferMapper_.getBuffers(data), Mesh::IndexVector{});

    std::vector<std::uint32_t> points;
    std::vector<std::uint32_t> lines;
    std::vector<std::uint32_t> triangles;

    auto it = data->NewCellIterator();
    for (it->InitTraversal(); !it->IsDoneWithTraversal(); it->GoToNextCell()) {
        if (cellMap[it->GetCellType()].dt == DrawType::Points) {
            vtkIdList* pointIds = it->GetPointIds();
            for (auto pit = pointIds->begin(); pit != pointIds->end(); ++pit) {
                points.push_back(static_cast<std::uint32_t>(*pit));
            }
        } else if (cellMap[it->GetCellType()].dt == DrawType::Lines) {
            vtkIdList* pointIds = it->GetPointIds();
            for (auto pit = pointIds->begin(); pit != pointIds->end(); ++pit) {
                lines.push_back(static_cast<std::uint32_t>(*pit));
            }
        } else if (cellMap[it->GetCellType()].dt == DrawType::Triangles) {
            switch (it->GetCellType()) {
                case VTK_TRIANGLE:
                    for (vtkIdType id : *it->GetPointIds()) {
                        triangles.push_back(static_cast<std::uint32_t>(id));
                    }
                    break;
                case VTK_TETRA: {
                    std::array<std::uint32_t, 4> ids;
                    int idx = 0;
                    for (vtkIdType id : *it->GetPointIds()) {
                        ids[idx++] = static_cast<std::uint32_t>(id);
                    }

                    // first
                    triangles.push_back(ids[1]);
                    triangles.push_back(ids[2]);
                    triangles.push_back(ids[3]);
                    // second
                    triangles.push_back(ids[0]);
                    triangles.push_back(ids[3]);
                    triangles.push_back(ids[2]);

                    triangles.push_back(ids[0]);
                    triangles.push_back(ids[2]);
                    triangles.push_back(ids[1]);

                    triangles.push_back(ids[0]);
                    triangles.push_back(ids[1]);
                    triangles.push_back(ids[3]);

                    break;
                }
                default:
                    break;
            }
        }
    }

    if (!points.empty()) {
        mesh->addIndices({DrawType::Points, ConnectivityType::None},
                         util::makeIndexBuffer(std::move(points)));
    }
    if (!lines.empty()) {
        mesh->addIndices({DrawType::Lines, ConnectivityType::None},
                         util::makeIndexBuffer(std::move(lines)));
    }
    if (!triangles.empty()) {
        mesh->addIndices({DrawType::Triangles, ConnectivityType::None},
                         util::makeIndexBuffer(std::move(triangles)));
    }

    if (mesh->getNumberOfIndicies() > 0) {
        outport_.setData(mesh);
    } else {
        outport_.setData(nullptr);
    }
}

void VTKToMesh::picking(PickingEvent* event) {
    const uint32_t id = static_cast<uint32_t>(event->getPickedId());

    // Show tooltip for current item
    if (event->getHoverState() == PickingHoverState::Enter) {
        event->setToolTip(fmt::format("{}", event->getPickedId()));

        BitSet highlight{id};
        brushLinkPort_.highlight(highlight);
    } else if (event->getHoverState() == PickingHoverState::Exit) {
        // unset tooltip at all times in case one was set prior disabling tooltips
        event->setToolTip("");
        brushLinkPort_.highlight({});
    }

    if ((event->getPressState() == PickingPressState::Release) &&
        (event->getPressItem() == PickingPressItem::Primary) && !event->getMovedSincePressed()) {

        BitSet selected(brushLinkPort_.getSelectedIndices());
        selected.flip(id);
        brushLinkPort_.select(selected);
        event->setUsed(true);
    }
}

}  // namespace inviwo
