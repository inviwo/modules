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

#ifdef _MSC_VER
#pragma optimize("", off)
#endif

#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/network/networklock.h>
#include <modules/tensorvisio/processors/vtkxmlrectilineargridreader.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLRectilinearGridReader.h>
#include <vtkXMLReader.h>
#include <vtkRectilinearGrid.h>
#include <vtkDataArray.h>
#include <warn/pop>

namespace inviwo {

// The Class Identifier has to be globally shared. Use a reverse DNS naming scheme
const ProcessorInfo VTKXMLRectilinearGridReader::processorInfo_{
    "org.inviwo.VTKXMLRectilinearGridReader",  // Class identifier
    "VTK XML Rectilinear Grid Reader",         // Display name
    "Data Input",                              // Category
    CodeState::Experimental,                   // Code state
    "VTK",                                     // Tags
};
const ProcessorInfo VTKXMLRectilinearGridReader::getProcessorInfo() const { return processorInfo_; }

VTKXMLRectilinearGridReader::VTKXMLRectilinearGridReader()
    : Processor()
    , file_("caseFile", "Case file", "", "ensight", InvalidationLevel::Valid)
    , outport_("outport")
    , loadArrays_("loadArrays", "Load arrays", InvalidationLevel::Valid)
    , loadData_("loadData", "Load data", InvalidationLevel::Valid)
    , arrays_("arrays", "Arrays", InvalidationLevel::Valid) {
    file_.clearNameFilters();
    file_.addNameFilter("VTK XML Rectilinear Grid (*.vtk)");

    addProperty(file_);
    addProperty(loadArrays_);
    addProperty(arrays_);
    addProperty(loadData_);

    loadArrays_.onChange([this] {
        if (file_.get() == "") {
            LogInfo("No case file selected");
            return;
        }

        auto reader = vtkSmartPointer<vtkXMLRectilinearGridReader>::New();

        reader->SetFileName(file_.get().c_str());

        reader->Update();

        auto grid = reader->GetOutput();

        auto points = grid->GetPointData();

        std::vector<std::string> keys;
        std::vector<std::string> identifiers;
        std::vector<int> values;

        for (int i = 0; i < points->GetNumberOfArrays(); i++) {
            auto c_name = points->GetArrayName(i);
            auto name = std::string(c_name);

            identifiers.push_back(name);

            std::transform(name.begin(), name.end(), name.begin(), ::tolower);

            keys.push_back(name);

            values.push_back(i);
        }

        NetworkLock lock;
        arrays_.replaceOptions(keys, identifiers, values);
    });

    loadData_.onChange([this] {
        if (file_.get().empty()) {
            LogInfo("No case file selected");
            return;
        }

        auto reader = vtkSmartPointer<vtkXMLRectilinearGridReader>::New();

        reader->SetFileName(file_.get().c_str());

        reader->Update();

        auto grid = reader->GetOutput();

        auto points = grid->GetPointData();

        double bounds[6];
        grid->GetBounds(bounds);

        auto extends = dvec3(bounds[1] - bounds[0], bounds[3] - bounds[2], bounds[5] - bounds[4]);

        auto tensors = points->GetArray(arrays_.getSelectedIdentifier().c_str());

        auto type = tensors->GetDataType();

        auto dimensionsPtr = grid->GetDimensions();

        size3_t dimensions(dimensionsPtr[0], dimensionsPtr[1], dimensionsPtr[2]);
        ivec3 iDimensions(dimensions);

        switch (type) {
            case 0:
                LogInfo("Data type void.");
                break;
            case 1:
                LogInfo("Data type bit.");
                break;
            case 2:
                LogInfo("Data type char.");
                break;
            case 3:
                LogInfo("Data type unsigned char.");
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;
            case 9:
                break;
            case 10:
                break;
            case 11:
                LogInfo("Data type double.");
                break;
            case 12:
                break;
            case 13:
                break;
            case 14:
                break;
            case 15:
                LogInfo("Data type signed char.");
                break;

            default:
                break;
        }

        double* tensor;

        auto outVec = std::vector<double>();

        auto size = dimensions.x * dimensions.z * dimensions.y;

        // TODO: find number of components or something here instead of hardcoding 9
        outVec.resize(size * 9);

        util::IndexMapper3D indexMapper(dimensions);

        for (int x = 0; x < iDimensions.x; x++) {
            for (int y = 0; y < iDimensions.y; y++) {
                for (int z = 0; z < iDimensions.z; z++) {
                    int coord[3] = {x, y, z};
                    auto id = grid->ComputePointId(coord);
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

        auto tensorField = std::make_shared<TensorField3D>(dimensions, outVec, extends);

        outport_.setData(tensorField);

        invalidate(InvalidationLevel::InvalidOutput);
    });

    addPort(outport_);
}

VTKXMLRectilinearGridReader::~VTKXMLRectilinearGridReader() {}

void VTKXMLRectilinearGridReader::initializeResources() {}

void VTKXMLRectilinearGridReader::process() {}

}  // namespace inviwo

#ifdef _MSC_VER
#pragma optimize("", on)
#endif
