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

#include <inviwo/vtk/datastructures/vtkdataset.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredPoints.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkArray.h>
#include <vtkImageData.h>
#include <warn/pop>

namespace inviwo {

std::optional<size3_t> VTKDataSet::getDimensions() const {
    ivec3 dims{0};

    switch (dataSet_->GetDataObjectType()) {
        case VTK_RECTILINEAR_GRID:
            vtkRectilinearGrid::SafeDownCast(dataSet_.GetPointer())
                ->GetDimensions(glm::value_ptr(dims));
            break;
        case VTK_STRUCTURED_GRID:
            vtkStructuredGrid::SafeDownCast(dataSet_.GetPointer())
                ->GetDimensions(glm::value_ptr(dims));
            break;
        case VTK_STRUCTURED_POINTS:
            vtkStructuredPoints::SafeDownCast(dataSet_.GetPointer())
                ->GetDimensions(glm::value_ptr(dims));
            break;
        case VTK_IMAGE_DATA:
            vtkImageData::SafeDownCast(dataSet_.GetPointer())->GetDimensions(glm::value_ptr(dims));
            break;
        default:
            return std::nullopt;
    }

    return size3_t{dims};
}

std::string VTKDataSet::getDataInfo() const {
    auto [cellNames, cellTypes] = [](const VTKDataSet& dataSet)
        -> std::pair<std::vector<std::string>, std::vector<std::string>> {
        std::vector<std::string> outNames{};
        std::vector<std::string> outTypes{};
        auto cellData = dataSet->GetCellData();
        if (!cellData) return {};
        for (int i = 0; i < cellData->GetNumberOfArrays(); ++i) {
            outNames.emplace_back(cellData->GetArray(i)->GetName());
            outTypes.emplace_back(cellData->GetArray(i)->GetClassName());
        }
        return {outNames, outTypes};
    }(*this);

    auto [pointNames, pointTypes] = [](const VTKDataSet& dataSet)
        -> std::pair<std::vector<std::string>, std::vector<std::string>> {
        std::vector<std::string> outNames{};
        std::vector<std::string> outTypes{};
        auto pointData = dataSet->GetPointData();
        if (!pointData) return {};
        for (int i = 0; i < pointData->GetNumberOfArrays(); ++i) {
            outNames.emplace_back(pointData->GetArray(i)->GetName());
            outTypes.emplace_back(pointData->GetArray(i)->GetClassName());
        }
        return {outNames, outTypes};
    }(*this);

    std::stringstream ss;
    ss << "<table border='0' cellspacing='0' cellpadding='0' "
          "style='border-color:white;white-space:pre;'>/n"
       << getHTMLTableRowString("Type", "VTK data set")
       << getHTMLTableRowString("Class name", std::string{dataSet_->GetClassName()});

    if (!cellNames.empty()) {
        ss << "</br>";
        ss << "</br>";

        ss << getHTMLTableIntermediateHeaderString("Cell data arrays");
        for (size_t j{0}; j < cellNames.size(); ++j) {
            ss << getHTMLTableRowString(cellNames[j], cellTypes[j]);
        }
    }

    if (!pointNames.empty()) {
        ss << "</br>";
        ss << "</br>";

        ss << getHTMLTableIntermediateHeaderString("Point data arrays");
        for (size_t j{0}; j < pointNames.size(); ++j) {
            ss << getHTMLTableRowString(pointNames[j], pointTypes[j]);
        }
    }

    ss << "</table>";
    return ss.str();
}
}  // namespace inviwo
