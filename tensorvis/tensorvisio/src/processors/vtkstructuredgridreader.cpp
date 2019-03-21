/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2018 Inviwo Foundation
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

#include <modules/tensorvisio/processors/vtkstructuredgridreader.h>
#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/util/indexmapper.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkPointLocator.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLStructuredGridReader.h>
#include <vtkStructuredGrid.h>
#include <vtkDoubleArray.h>
#include <warn/pop>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKStructuredGridReader::processorInfo_{
    "org.inviwo.VTKStructuredGridReader",  // Class identifier
    "VTK Structured Grid XML Reader",          // Display name
    "Tensor field IO",                     // Category
    CodeState::Experimental,               // Code state
    Tags::None,                            // Tags
};
const ProcessorInfo VTKStructuredGridReader::getProcessorInfo() const { return processorInfo_; }

VTKStructuredGridReader::VTKStructuredGridReader()
    : Processor()
    , file_("file", "File")
    , outport_("outport")
    , normalizeBasis_("normalizeBasis", "Normalize basis", true)
    , invalidate_("invalidate", "Invalidate") {
    addProperty(file_);

    file_.addNameFilter("VTK Structured Grid XML (*.vts)");

    addPort(outport_);

    addProperty(normalizeBasis_);

    addProperty(invalidate_);
}

void VTKStructuredGridReader::process() {
    if (file_.get() == "") {
        LogInfo("No case file selected");
        return;
    }
    if (!filesystem::fileExists(file_.get())) {
        LogWarn("Could not find file: " << file_.get());
        return;
    }

    auto reader = vtkSmartPointer<vtkXMLStructuredGridReader>::New();

    reader->SetFileName(file_.get().c_str());

    reader->Update();

    auto grid = reader->GetOutput();

    auto pointData = grid->GetPointData();

    auto tensors = pointData->GetArray("stress_1.0");

    auto dimensionsPtr = grid->GetDimensions();

    size3_t dimensions(dimensionsPtr[0], dimensionsPtr[1], dimensionsPtr[2]);

    // Building locator
    auto locator = vtkSmartPointer<vtkPointLocator>::New();
    locator->SetDataSet(grid);
    locator->BuildLocator();

    auto bounds = grid->GetBounds();
    auto extends = dvec3(bounds[1] - bounds[0], bounds[3] - bounds[2], bounds[5] - bounds[4]);

    auto stepSize = extends / dvec3(dimensions - size3_t(1));

    util::IndexMapper3D indexMapper(dimensions);

    auto size = dimensions.x * dimensions.z * dimensions.y;

    std::vector<double> tensorVector;
    tensorVector.resize(size * 9);

    for (size_t z = 0; z < dimensions.z; z++) {
        const auto zCoord = z * stepSize.z;
        for (size_t y = 0; y < dimensions.y; y++) {
            const auto yCoord = y * stepSize.y;
            for (size_t x = 0; x < dimensions.x; x++) {
                const auto xCoord = x * stepSize.x;
                auto index = indexMapper(size3_t(x, y, z));
                // Finding point
                const double pt[3] = {xCoord, yCoord, zCoord};
                vtkIdType id = locator->FindClosestPoint(pt);

                auto tensor = tensors->GetTuple9(id);

                tensorVector[index] = tensor[0];
                tensorVector[index + 1] = tensor[1];
                tensorVector[index + 2] = tensor[2];

                tensorVector[index + 3] = tensor[3];
                tensorVector[index + 4] = tensor[4];
                tensorVector[index + 5] = tensor[5];

                tensorVector[index + 6] = tensor[6];
                tensorVector[index + 7] = tensor[7];
                tensorVector[index + 8] = tensor[8];
            }
        }
    }

    if (normalizeBasis_.get()) {
        auto max = std::max(std::max(extends.x, extends.y), extends.z);
        extends /= max;
    }

    outport_.setData(std::make_shared<TensorField3D>(dimensions, tensorVector, extends));
}

}  // namespace inviwo
