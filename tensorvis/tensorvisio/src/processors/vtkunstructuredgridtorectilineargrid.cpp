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

#include <modules/tensorvisio/processors/vtkunstructuredgridtorectilineargrid.h>
#include <modules/vtk/util/lambda2func.h>
#include <inviwo/core/util/filesystem.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkRectilinearGrid.h>
#include <vtkXMLRectilinearGridWriter.h>
#include <vtkProbeFilter.h>
#include <vtkCallbackCommand.h>
#include <vtkObject.h>
#include <vtkCellTreeLocator.h>
#include <vtkXMLWriter.h>
#include <warn/pop>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKUnstructuredGridToRectilinearGrid::processorInfo_{
    "org.inviwo.VTKUnstructuredGridToRectilinearGrid",  // Class identifier
    "VTK Unstructured Grid To Rectilinear Grid",        // Display name
    "Tensor",                                           // Category
    CodeState::Experimental,                            // Code state
    Tags::None,                                         // Tags
};
const ProcessorInfo VTKUnstructuredGridToRectilinearGrid::getProcessorInfo() const {
    return processorInfo_;
}

VTKUnstructuredGridToRectilinearGrid::VTKUnstructuredGridToRectilinearGrid()
    : Processor()
    , file_("file", "File", "", "VTK", InvalidationLevel::Valid)
    , outFile_("outFile", "Output file", "", "VTK", InvalidationLevel::Valid)
    , dimensions_("dimensions", "Output dimensions", ivec3(32), ivec3(2), ivec3(2048))
    , button_("button", "Read", InvalidationLevel::Valid) {
    addProperty(file_);
    addProperty(outFile_);
    addProperty(dimensions_);
    addProperty(button_);

    file_.clearNameFilters();
    file_.addNameFilter("VTK Unstructured Grid (*.vtk)");
    file_.addNameFilter("VTK Unstructured Grid (*.vtu)");

    button_.onChange([this]() { loadData(); });
}

void VTKUnstructuredGridToRectilinearGrid::process() {}

void VTKUnstructuredGridToRectilinearGrid::loadData() {
    if (file_.get() == "") {
        LogInfo("No case file selected");
        return;
    }

    auto reader = vtkSmartPointer<vtkUnstructuredGridReader>::New();

    reader->SetFileName(file_.get().c_str());

    std::cout << "Reading." << std::endl;
    reader->Update();
    std::cout << "Done reading." << std::endl;

    auto unstructuredGrid = reader->GetOutput();

    auto bounds = unstructuredGrid->GetBounds();

    auto gridSize = dimensions_.get();
    const auto gridBounds = dvec3(gridSize - ivec3(1));

    // auto& pb = progressBar_;

    // pb.show();

    auto fn = fnptr<void(vtkObject*, long unsigned int, void*, void*)>(
        [/*&pb*/](vtkObject* caller, long unsigned int eventId, void* clientData,
                  void* callData) -> void {
            vtkProbeFilter* testFilter = static_cast<vtkProbeFilter*>(caller);
            std::cout << std::fixed << std::setprecision(6);
            std::cout << "Probing progress: " << testFilter->GetProgress() * 100. << "%"
                      << std::endl;

            // pb.updateProgress(testFilter->GetProgress());
        });

    // pb.hide();

    vtkSmartPointer<vtkCallbackCommand> progressCallback =
        vtkSmartPointer<vtkCallbackCommand>::New();
    progressCallback->SetCallback(fn);

    std::cout << "Generating grid." << std::endl;
    auto pointSet = vtkSmartPointer<vtkRectilinearGrid>::New();
    pointSet->SetDimensions(glm::value_ptr(gridSize));

    auto xCoordinates = vtkSmartPointer<vtkDoubleArray>::New();
    auto yCoordinates = vtkSmartPointer<vtkDoubleArray>::New();
    auto zCoordinates = vtkSmartPointer<vtkDoubleArray>::New();

    const auto xRange = std::abs(bounds[0] - bounds[1]);
    const auto yRange = std::abs(bounds[2] - bounds[3]);
    const auto zRange = std::abs(bounds[4] - bounds[5]);

    const auto stepSize =
        dvec3(xRange / gridBounds.x, yRange / gridBounds.y, zRange / gridBounds.z);

    for (int z = 0; z < gridSize.z; z++) {
        const auto zCoord = bounds[4] + static_cast<double>(z) * stepSize.z;
        zCoordinates->InsertNextValue(zCoord);
    }
    for (int y = 0; y < gridSize.y; y++) {
        const auto yCoord = bounds[2] + static_cast<double>(y) * stepSize.y;
        yCoordinates->InsertNextValue(yCoord);
    }
    for (int x = 0; x < gridSize.x; x++) {
        const auto xCoord = bounds[0] + static_cast<double>(x) * stepSize.x;
        xCoordinates->InsertNextValue(xCoord);
    }

    pointSet->SetXCoordinates(xCoordinates);
    pointSet->SetYCoordinates(yCoordinates);
    pointSet->SetZCoordinates(zCoordinates);

    std::cout << "Done generating grid." << std::endl;

    vtkSmartPointer<vtkCellTreeLocator> cellLocator = vtkSmartPointer<vtkCellTreeLocator>::New();
    cellLocator->SetDataSet(pointSet);
    std::cout << "Building Cell Tree Locator." << std::endl;
    cellLocator->BuildLocator();
    std::cout << "Done building Cell Tree Locator." << std::endl;

    auto probeFilter = vtkSmartPointer<vtkProbeFilter>::New();
    probeFilter->SetSourceData(unstructuredGrid);
    probeFilter->SetCellLocatorPrototype(cellLocator);
    probeFilter->SetInputData(pointSet);

    probeFilter->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    std::cout << "Probing." << std::endl;
    probeFilter->Update();
    std::cout << "Done probing." << std::endl;
    auto rectilieanrGrid = probeFilter->GetRectilinearGridOutput();

    // Write file
    auto writer_xml = vtkSmartPointer<vtkXMLRectilinearGridWriter>::New();
    writer_xml->SetFileName(outFile_.get().c_str());
    writer_xml->SetInputData(rectilieanrGrid);
    std::cout << "Writing." << std::endl;
    writer_xml->Write();
    std::cout << "Done writing." << std::endl;
}
}  // namespace inviwo
