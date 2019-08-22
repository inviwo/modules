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

#include <inviwo/vtk/processors/vtkunstructuredgridtorectilineargrid.h>
#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/common/inviwoapplication.h>

#include <inviwo/core/util/clock.h>
#include <inviwo/vtk/util/vtkutil.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkProbeFilter.h>
#include <vtkCallbackCommand.h>
#include <vtkCellTreeLocator.h>
#include <vtkRectilinearGrid.h>
#include <warn/pop>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKUnstructuredGridToRectilinearGrid::processorInfo_{
    "org.inviwo.VTKUnstructuredGridToRectilinearGrid",  // Class identifier
    "VTK Unstructured Grid To Rectilinear Grid",        // Display name
    "VTK",                                              // Category
    CodeState::Experimental,                            // Code state
    Tags::CPU,                                          // Tags
};
const ProcessorInfo VTKUnstructuredGridToRectilinearGrid::getProcessorInfo() const {
    return processorInfo_;
}

VTKUnstructuredGridToRectilinearGrid::VTKUnstructuredGridToRectilinearGrid()
    : Processor()
    , ActivityIndicatorOwner()
    , maxDimension_("maxDimension", "Max output dimension", 32, 2, 2048)
    , button_("button", "Convert Data")
    , abortButton_("abortButton", "Abort Conversion", InvalidationLevel::Valid)
    , inport_("inport")
    , outport_("outport")
    , state_(std::make_shared<WorkerState>()) {
    addProperty(maxDimension_);
    addProperty(button_);
    addProperty(abortButton_);

    button_.onChange([this]() {
        if (state_->state != State::Working) {
            loadData();
        }
    });
    abortButton_.onChange([this]() {
        if (state_->state == State::Working) {
            state_->abortConversion_ = true;
        }
    });
    progressBar_.hide();

    addPort(inport_);
    addPort(outport_);
}

VTKUnstructuredGridToRectilinearGrid::~VTKUnstructuredGridToRectilinearGrid() {
    state_->processorExists_ = false;
    state_->abortConversion_ = true;
}

void VTKUnstructuredGridToRectilinearGrid::process() {}

void VTKUnstructuredGridToRectilinearGrid::loadData() {
    state_->abortConversion_ = false;

    const auto done = [this, state = state_]() {
        if (state->processorExists_) {
            dispatchFront([this]() {
                progressBar_.hide();
                getActivityIndicator().setActive(false);
            });
        }
    };
    const auto abort = [state = state_, done]() {
        state->state = State::Abort;
        LogWarnCustom("VTK conversion", "Conversion aborted.");
        done();
    };

    auto grid = **inport_.getData();

    if (grid->GetDataObjectType() != VTK_UNSTRUCTURED_GRID) {
        LogError("Input is not an unstructured grid but a " + std::string{grid->GetClassName()} +
                 ". Aborting.");
        return;
    }

    auto unstructuredGrid = vtkUnstructuredGrid::SafeDownCast(grid);

    auto bounds = unstructuredGrid->GetBounds();

    const auto xRange = std::abs(bounds[0] - bounds[1]);
    const auto yRange = std::abs(bounds[2] - bounds[3]);
    const auto zRange = std::abs(bounds[4] - bounds[5]);

    const auto largestRange = std::max(xRange, std::max(yRange, zRange));

    const auto maxDim = maxDimension_.get();

    const int xDim = static_cast<int>((xRange / largestRange) * maxDim);
    const int yDim = static_cast<int>((yRange / largestRange) * maxDim);
    const int zDim = static_cast<int>((zRange / largestRange) * maxDim);

    state_->dims_ = {xDim, yDim, zDim};

    auto dispatch = [this, done, abort, unstructuredGrid,
                     activityIndicator = &getActivityIndicator(),
                     state = state_]() mutable -> void {
        auto updateActivity = [activityIndicator, state](bool active) {
            if (state->processorExists_) {
                dispatchFront([active](ActivityIndicator* i) { i->setActive(active); },
                              activityIndicator);
            }
        };

        updateActivity(true);
        this->state_->state = State::Working;

        if (this->state_->abortConversion_) {
            abort();
            return;
        }

        const auto bounds = unstructuredGrid->GetBounds();
        const auto gridSize = this->state_->dims_;
        const auto gridBounds = dvec3(gridSize - ivec3(1));

        vtkSmartPointer<vtkCallbackCommand> progressCallback =
            vtkSmartPointer<vtkCallbackCommand>::New();
        progressCallback->SetCallback(vtkProgressBarCallback);
        progressCallback->SetClientData(&progressBar_);

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

        if (this->state_->abortConversion_) {
            abort();
            return;
        }

        vtkSmartPointer<vtkCellTreeLocator> cellLocator =
            vtkSmartPointer<vtkCellTreeLocator>::New();
        cellLocator->SetDataSet(pointSet);
        cellLocator->BuildLocator();

        if (this->state_->abortConversion_) {
            abort();
            return;
        }

        auto probeFilter = vtkSmartPointer<vtkProbeFilter>::New();
        probeFilter->SetSourceData(unstructuredGrid);
        probeFilter->SetCellLocatorPrototype(cellLocator);
        probeFilter->SetInputData(pointSet);
        probeFilter->AddObserver(vtkCommand::ProgressEvent, progressCallback);

        probeFilter->Update();

        if (this->state_->abortConversion_) {
            abort();
            return;
        }

        dataSet_ = probeFilter->GetRectilinearGridOutput();

        done();
        updateActivity(false);
        this->state_->state = State::Done;

        dispatchFront([this]() {
            data_ = std::make_shared<VTKDataSet>(dataSet_);
            outport_.setData(data_);
        });
    };

    state_->state = State::NotStarted;

    dispatchPool(dispatch);
}

}  // namespace inviwo
