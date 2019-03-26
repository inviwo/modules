/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019 Inviwo Foundation
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

#include <modules/tensorvisio/processors/vtkreader.h>
#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/util/clock.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKReader::processorInfo_{
    "org.inviwo.VTKReader",   // Class identifier
    "VTK Reader",             // Display name
    "VTK",                    // Category
    CodeState::Experimental,  // Code state
    Tags::CPU,                // Tags
};
const ProcessorInfo VTKReader::getProcessorInfo() const { return processorInfo_; }

VTKReader::VTKReader()
    : Processor()
    , file_("vtkFile", "VTK file", "", "VTK")
    , reloadButton_("reload", "Reload Data")
    , outport_("VTKDataObjectOutport") {

    file_.addNameFilter("Serial vtkImageData (structured) (.vti)");
    file_.addNameFilter("Serial vtkPolyData (unstructured) (.vtp)");
    file_.addNameFilter("Serial vtkRectilinearGrid (structured) (.vtr)");
    file_.addNameFilter("Serial vtkStructuredGrid (structured) (.vts)");
    file_.addNameFilter("Serial vtkUnstructuredGrid (unstructured) (.vtu)");
    file_.addNameFilter("Parallel vtkImageData (structured) (.pvti)");
    file_.addNameFilter("Parallel vtkPolyData (unstructured) (.pvtp)");
    file_.addNameFilter("Parallel vtkRectilinearGrid (structured) (.pvtr)");
    file_.addNameFilter("Parallel vtkStructuredGrid (structured) (.pvts)");
    file_.addNameFilter("Parallel vtkUnstructuredGrid (unstructured) (.pvtu)");
    file_.addNameFilter("Legacy (*vtk)");

    addProperty(file_);

    addPort(outport_);

    reloadButton_.onChange([this]() { reader_ = nullptr; });
}

void VTKReader::process() {
    if (!filesystem::fileExists(file_)) {
        return;
    }
    if (!reader_ || file_.isModified()) {
        createReader();
    }

    outport_.setData(std::make_shared<vtkDataObject*>(reader_->GetOutput()));
}

void VTKReader::createReader() {
    reader_ = vtkSmartPointer<vtkXMLGenericDataObjectReader>::New();
    reader_->SetFileName(file_.get().c_str());
    reader_->Update();
}

}  // namespace inviwo
