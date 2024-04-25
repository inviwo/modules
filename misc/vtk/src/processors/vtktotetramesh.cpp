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

#include <inviwo/vtk/processors/vtktotetramesh.h>
#include <inviwo/vtk/datastructures/vtktetramesh.h>
#include <inviwo/tetramesh/util/tetrameshutils.h>

#include <vtkUnstructuredGrid.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKToTetraMesh::processorInfo_{
    "org.inviwo.VTKToTetraMesh",                   // Class identifier
    "VTK To TetraMesh",                            // Display name
    "Unstructured Grids",                          // Category
    CodeState::Experimental,                       // Code state
    Tags::CPU | Tag{"VTK"} | Tag{"Unstructured"},  // Tags
    R"(Use a VTK Unstructured Grid dataSet as input for an Inviwo TetraMesh)"_unindentHelp};

const ProcessorInfo VTKToTetraMesh::getProcessorInfo() const { return processorInfo_; }

VTKToTetraMesh::VTKToTetraMesh()
    : Processor{}
    , inport_{"vtkdata", VTK_UNSTRUCTURED_GRID,
              "VTK dataset. Only unstructured grids are accepted."_help}
    , outport_{"tetramesh", "Tetrahedral mesh information extracted from the VTK dataset."_help}
    , bufferMapper_{*this, utilvtk::ArrayUsageSelection(utilvtk::ArrayUsage::Scalar)} {

    addPorts(inport_, outport_);
}

void VTKToTetraMesh::process() {
    if (inport_.isChanged()) {
        bufferMapper_.updateSources(vtkDataSet::SafeDownCast(inport_.getData()));
    }

    auto vtkgrid = vtkUnstructuredGrid::SafeDownCast(inport_.getData());
    auto tetraMesh = std::make_shared<VTKTetraMesh>(bufferMapper_, vtkgrid);

    outport_.setData(tetraMesh);
}

}  // namespace inviwo
