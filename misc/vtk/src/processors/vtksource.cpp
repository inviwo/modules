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

#include <inviwo/vtk/processors/vtksource.h>

#include <vtkDataSetReader.h>
// #include <vtkXMLReader.h>
#include <vtkDataSet.h>
#include <vtkDataObject.h>
#include <vtkSmartPointer.h>

namespace inviwo::vtk {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKSource::processorInfo_{"org.inviwo.VTKSource",   // Class identifier
                                              "VTK Source",             // Display name
                                              "Data Input",             // Category
                                              CodeState::Experimental,  // Code state
                                              Tags::CPU,                // Tags
                                              R"(Loads VTK data from a given file)"_unindentHelp};

const ProcessorInfo VTKSource::getProcessorInfo() const { return processorInfo_; }

VTKSource::VTKSource(const std::filesystem::path& filePath)
    : Processor{}
    , outport_{"outport", VTK_DATA_SET, "VTK dataset"_help}
    , file_{"filename", "File Name", "File name of the VTK dataset"_help, filePath, "vtk"}
    , reload_{"reload", "Reload data"} {

    addPorts(outport_);
    addProperties(file_, reload_);

    reader_ = vtkDataSetReader::New();
}

VTKSource::~VTKSource() { reader_->Delete(); }

void VTKSource::process() {
    if (file_.get().empty()) return;

    reader_->SetFileName(file_.get().string().c_str());
    // mark reader as modified to force a reload even if the same file name has been used before
    reader_->Modified();
    reader_->Update();
    reader_->CloseVTKFile();

    outport_.setData(reader_->GetOutput());
}

}  // namespace inviwo::vtk
