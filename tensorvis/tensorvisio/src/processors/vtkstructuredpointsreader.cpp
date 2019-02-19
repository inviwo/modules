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

#include <modules/tensorvisio/processors/vtkstructuredpointsreader.h>
#include <inviwo/core/util/filesystem.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkStructuredPoints.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredPointsReader.h>
#include <warn/pop>

namespace inviwo {

// The Class Identifier has to be globally shared. Use a reverse DNS naming scheme
const ProcessorInfo VTKStructuredPointsReader::processorInfo_{
    "org.inviwo.VTKStructuredPointsReader",  // Class identifier
    "VTK Structured Points Reader",          // Display name
    "Data Input",                            // Category
    CodeState::Experimental,                 // Code state
    "VTK",                                   // Tags
};
const ProcessorInfo VTKStructuredPointsReader::getProcessorInfo() const { return processorInfo_; }

VTKStructuredPointsReader::VTKStructuredPointsReader()
    : Processor(), file_("caseFile", "Case file", "", "ensight"), outport_("outport") {
    addProperty(file_);

    file_.clearNameFilters();
    file_.addNameFilter("VTK Structured Points (*.vtk)");

    addPort(outport_);
}

VTKStructuredPointsReader::~VTKStructuredPointsReader() {}

void VTKStructuredPointsReader::process() { loadData(); }

void VTKStructuredPointsReader::loadData() {
    if (file_.get() == "") {
        LogInfo("No case file selected");
        return;  // no file selected
    }
    if (!filesystem::fileExists(file_.get())) {
        LogWarn("Could not find file: " << file_.get());
        return;
    }

    vtkSmartPointer<vtkStructuredPointsReader> reader;

    reader = vtkStructuredPointsReader::New();

    reader->SetFileName(file_.get().c_str());

    reader->Update();

    auto structuredPoints = reader->GetOutput();

    auto bounds = new double[6];

    structuredPoints->GetBounds(bounds);

    const auto extends = dvec3(bounds[0]-bounds[1], bounds[2] - bounds[3], bounds[4] - bounds[5]);

    auto points = structuredPoints->GetPointData();

    auto tensors = points->GetTensors();

    auto dimensionsPtr = structuredPoints->GetDimensions();

    size3_t dimensions(dimensionsPtr[0], dimensionsPtr[1], dimensionsPtr[2]);

    std::shared_ptr<std::pair<size3_t, std::vector<double>>> outPair =
        std::make_shared<std::pair<size3_t, std::vector<double>>>();

    auto& outDimensions = outPair->first;
    outDimensions = dimensions;

    auto& outVec = outPair->second;
    outVec.resize(dimensions.x * dimensions.y * dimensions.z * 9);

    util::IndexMapper3D indexMapper(dimensions);

    double* tensor;

    for (size_t x = 0; x < dimensions.x; x++) {
        for (size_t y = 0; y < dimensions.y; y++) {
            for (size_t z = 0; z < dimensions.z; z++) {
                int coord[3] = {((int) x), ((int) y), ((int) z)};
                auto id = structuredPoints->ComputePointId(coord);

                auto index = indexMapper(size3_t(x, y, z)) * 9;

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

    outport_.setData(std::make_shared<TensorField3D>(dimensions, outVec, extends));
}

}  // namespace inviwo
