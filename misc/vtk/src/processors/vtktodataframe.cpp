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

#include <inviwo/vtk/processors/vtktodataframe.h>

#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/dataframe/datastructures/column.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/util/detected.h>

#include <vtkType.h>

#include <vtkDataObject.h>
#include <vtkDataSet.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkAbstractArray.h>
#include <vtkArrayDispatch.h>
#include <vtkDataArrayMeta.h>
#include <vtkCell.h>
#include <vtkCellType.h>
#include <vtkCellTypes.h>

namespace inviwo {

template <typename T>
using RangeType = decltype(std::declval<T>()->getRange(std::declval<int>()));

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKToDataFrame::processorInfo_{
    "org.inviwo.VTKToDataFrame",                // Class identifier
    "VTK To Data Frame",                        // Display name
    "VTK",                                      // Category
    CodeState::Experimental,                    // Code state
    Tags::CPU | Tag{"VTK"} | Tag{"DataFrame"},  // Tags
    R"(Convert VTK DataSet cell and point array data to dataframes)"_unindentHelp};

const ProcessorInfo VTKToDataFrame::getProcessorInfo() const { return processorInfo_; }

VTKToDataFrame::VTKToDataFrame()
    : Processor{}
    , inport_{"inport", VTK_DATA_SET, ""_help}
    , cells_{"cells", ""_help}
    , points_{"points", ""_help} {

    addPorts(inport_, cells_, points_);
}

void addVTKDataArrayToDataFrame(vtkDataArray* array, DataFrame& df) {
    auto worker = [&](auto* typedArray) {
        using DType = decltype(typedArray->GetTypedComponent(0, 0));

        const auto nTuples = typedArray->GetNumberOfTuples();

        for (int comp = 0; comp < typedArray->GetNumberOfComponents(); ++comp) {
            std::vector<DType> coldata(nTuples);
            for (int i = 0; i < nTuples; ++i) {
                coldata[i] = typedArray->GetTypedComponent(i, comp);
            }

            auto name = [&]() -> std::string {
                if (typedArray->GetNumberOfComponents() == 1) {
                    return typedArray->GetName();
                } else if (typedArray->HasAComponentName()) {
                    return fmt::format("{} {}", typedArray->GetName(),
                                       typedArray->GetComponentName(comp));
                } else {
                    return fmt::format("{} {}", typedArray->GetName(), comp);
                }
            }();

            std::optional<dvec2> range;
            if constexpr (util::is_detected_exact_v<double*, RangeType, decltype(typedArray)>) {
                range = glm::dvec2{typedArray->getRange(comp)[0], typedArray->getRange(comp)[1]};
            }

            df.addColumn(name, std::move(coldata), Unit{}, range);
        }
    };
    if (!vtkArrayDispatch::Dispatch::Execute(array, worker)) {
        throw Exception("no matching type");
    }
}

void addVTKCellsToDataFrame(vtkDataSet* dataSet, DataFrame& df) {
    const auto nCells = dataSet->GetNumberOfCells();

    std::vector<std::uint32_t> cellType(nCells);
    std::vector<vtkIdType> nPoints(nCells);

    for (vtkIdType cellId = 0; cellId < nCells; ++cellId) {
        auto* cell = dataSet->GetCell(cellId);

        cellType[cellId] = static_cast<std::uint32_t>(cell->GetCellType());
        nPoints[cellId] = cell->GetNumberOfPoints();
    }

    std::vector<std::string> types;
    for (int i = 0; i < VTK_NUMBER_OF_CELL_TYPES; ++i) {
        types.emplace_back(vtkCellTypes::GetClassNameFromTypeId(i));
    }

    df.addColumn(
        std::make_shared<CategoricalColumn>("Cell Type", std::move(cellType), std::move(types)));

    df.addColumn("#Points", std::move(nPoints));
}

void addVTKPointsToDataFrame(vtkDataSet* dataSet, DataFrame& df) {
    const auto nPoints = dataSet->GetNumberOfPoints();

    std::vector<double> x(nPoints);
    std::vector<double> y(nPoints);
    std::vector<double> z(nPoints);

    for (vtkIdType pointId = 0; pointId < nPoints; ++pointId) {
        auto* point = dataSet->GetPoint(pointId);
        x[pointId] = point[0];
        y[pointId] = point[1];
        z[pointId] = point[2];
    }

    df.addColumn("x", std::move(x));
    df.addColumn("y", std::move(y));
    df.addColumn("z", std::move(z));
}

void VTKToDataFrame::process() {
    auto data = vtkDataSet::SafeDownCast(inport_.getData());

    {
        auto cellData = data->GetCellData();
        auto cellDf = std::make_shared<DataFrame>();
        addVTKCellsToDataFrame(data, *cellDf);
        const auto narrays = cellData->GetNumberOfArrays();
        for (int i = 0; i < narrays; ++i) {
            if (auto array = cellData->GetArray(i)) {
                addVTKDataArrayToDataFrame(array, *cellDf);
            }
        }
        cellDf->updateIndexBuffer();
        cells_.setData(cellDf);
    }

    {
        auto pointData = data->GetPointData();
        auto pointDf = std::make_shared<DataFrame>();
        addVTKPointsToDataFrame(data, *pointDf);
        const auto narrays = pointData->GetNumberOfArrays();
        for (int i = 0; i < narrays; ++i) {
            if (auto array = pointData->GetArray(i)) {
                addVTKDataArrayToDataFrame(array, *pointDf);
            }
        }
        pointDf->updateIndexBuffer();
        points_.setData(pointDf);
    }
}

}  // namespace inviwo
