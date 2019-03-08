/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2018 Inviwo Foundation
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

#include <modules/tensorvisio/processors/vtkrectilineargridreader.h>
#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/util/indexmapper.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkRectilinearGridReader.h>
#include <vtkRectilinearGrid.h>
#include <vtkDataArray.h>
#include <warn/pop>

namespace inviwo {

// The Class Identifier has to be globally shared. Use a reverse DNS naming scheme
const ProcessorInfo VTKRectilinearGridReader::processorInfo_{
    "org.inviwo.VTKRectilinearGridReader",  // Class identifier
    "VTK Rectilinear Grid Reader",          // Display name
    "Data Input",                           // Category
    CodeState::Experimental,                // Code state
    "VTK",                                  // Tags
};
const ProcessorInfo VTKRectilinearGridReader::getProcessorInfo() const { return processorInfo_; }

VTKRectilinearGridReader::VTKRectilinearGridReader()
    : Processor(), file_("caseFile", "Case file", "", "ensight"), outport_("outport") {

    addPort(outport_);
    addProperty(file_);

    file_.addNameFilter("VTK Rectilinear Grid File (*.vtr)");
}

VTKRectilinearGridReader::~VTKRectilinearGridReader() {}

void VTKRectilinearGridReader::process() {
    if (!filesystem::fileExists(file_)) {
        return;
    }

    auto reader = vtkSmartPointer<vtkRectilinearGridReader>::New();

    reader->SetFileName(file_.get().c_str());

    reader->Update();

    auto grid = reader->GetOutput();

    auto points = grid->GetPointData();

    double bounds[6];
    grid->GetBounds(bounds);

    auto extends = dvec3(bounds[1] - bounds[0], bounds[3] - bounds[2], bounds[5] - bounds[4]);

    auto tensors = points->GetArray("stress_1.0");

    auto dimensionsPtr = grid->GetDimensions();

    size3_t dimensions(dimensionsPtr[0], dimensionsPtr[1], dimensionsPtr[2]);

    double* tensor;

    auto outVec = std::vector<double>();

    auto size = dimensions.x * dimensions.z * dimensions.y;

    // TODO: find number of components or something here instead of hardcoding 9
    outVec.resize(size * 9);

    util::IndexMapper3D indexMapper(dimensions);

    for (size_t x = 0; x < dimensions.x; x++) {
        for (size_t y = 0; y < dimensions.y; y++) {
            for (size_t z = 0; z < dimensions.z; z++) {
                int coord[3] = {int(x), int(y), int(z)};
                auto id = grid->ComputePointId(coord);
                auto index = indexMapper(x, y, z) * 9;

                tensor = tensors->GetTuple9(id);

                outVec[index] = tensor[0];
                outVec[index + 1] = tensor[1];
                outVec[index + 2] = tensor[2];

                outVec[index + 3] = tensor[3];
                outVec[index + 4] = tensor[4];
                outVec[index + 5] = tensor[5];

                outVec[index + 6] = tensor[6];
                outVec[index + 7] = tensor[7];
                outVec[index + 8] = tensor[8];
            }
        }
    }

    auto tensorField = std::make_shared<TensorField3D>(dimensions, outVec, extends);
    tensorField->setOffset({bounds[0], bounds[2], bounds[4]});

    outport_.setData(tensorField);
}

}  // namespace inviwo
