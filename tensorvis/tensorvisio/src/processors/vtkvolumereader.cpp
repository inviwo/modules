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

#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <modules/tensorvisio/processors/vtkvolumereader.h>
#include <modules/base/algorithm/volume/volumeramsubsample.h>

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
const ProcessorInfo VTKVolumeReader::processorInfo_{
    "org.inviwo.VTKVolumeReader",  // Class identifier
    "VTK Volume Reader",           // Display name
    "Data Input",                  // Category
    CodeState::Experimental,       // Code state
    "VTK",                         // Tags
};
const ProcessorInfo VTKVolumeReader::getProcessorInfo() const { return processorInfo_; }

VTKVolumeReader::VTKVolumeReader()
    : Processor(), file_("caseFile", "Case file", "", "ensight"), outport_("outport") {
    addProperty(file_);

    file_.clearNameFilters();
    file_.addNameFilter("VTK Structured Points (*.vtk)");

    addPort(outport_);
}

VTKVolumeReader::~VTKVolumeReader() {}

void VTKVolumeReader::process() { loadData(); }

void VTKVolumeReader::loadData() {
    if (file_.get() == "") {
        LogInfo("No case file selected");
        return;  // no file selected
    }
    if (!filesystem::fileExists(file_.get())) {
        LogWarn("Could not find file: " << file_.get());
        return;
    }

    auto reader = vtkStructuredPointsReader::New();

    reader->SetFileName(file_.get().c_str());

    reader->Update();

    auto structuredPoints = reader->GetOutput();

    auto bounds = new double[6];

    structuredPoints->GetBounds(bounds);

    auto points = structuredPoints->GetPointData();

    auto tensors = points->GetScalars();

    auto dimensionsPtr = structuredPoints->GetDimensions();

    size3_t dimensions(dimensionsPtr[0], dimensionsPtr[1], dimensionsPtr[2]);
    ivec3 iDimensions(dimensions);

    std::vector<float> data;

    data.resize(dimensions.x * dimensions.y * dimensions.z);

    util::IndexMapper3D indexMapper(dimensions);

    float scalar;

    for (int z = 0; z < iDimensions.z; z++) {
        for (int y = 0; y < iDimensions.y; y++) {
            for (int x = 0; x < iDimensions.x; x++) {
                auto index = indexMapper(size3_t(x, y, z));

                scalar = float(tensors->GetTuple1(vtkIdType(index)));

                data[index] = scalar;
            }
        }
    }

    auto volume = std::make_shared<Volume>(dimensions, DataFloat32::get());
    auto volumeRAM = static_cast<float*>(volume->getEditableRepresentation<VolumeRAM>()->getData());
    memcpy(volumeRAM, data.data(), data.size() * sizeof(float));
    volume->dataMap_.dataRange = dvec2(0, 1);
    volume->dataMap_.valueRange = dvec2(0, 1);

    outport_.setData(volume);
}

}  // namespace inviwo
