/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2025 Inviwo Foundation
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

#include <inviwo/tensorvisio/processors/vtktotensorfield2d.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/util/glm.h>
#include <inviwo/core/util/formats.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/utilities.h>

#include <inviwo/tensorvisbase/datastructures/tensorfield2d.h>
#include <inviwo/tensorvisbase/util/misc.h>

#include <inviwo/vtk/util/vtkdatautils.h>

#include <fmt/core.h>

#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkAbstractArray.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredPoints.h>
#include <vtkArrayDispatch.h>
#include <vtkDataObjectTypes.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKToTensorField2D::processorInfo_{
    "org.inviwo.VTKToTensorField2D",         // Class identifier
    "VTK To TensorField 2D",                 // Display name
    "VTK",                                   // Category
    CodeState::Experimental,                 // Code state
    Tags::CPU | Tag{"VTK"} | Tag{"Tensor"},  // Tags
    R"(Converts a VTK data set to a 2D tensor field. This requires that one of the dimensions 
    of the VTK data set is 1. Additionally, the data type must be either float or double. 
    Lastly, unstructured grids are not supported.)"_unindentHelp,
};
const ProcessorInfo& VTKToTensorField2D::getProcessorInfo() const { return processorInfo_; }

constexpr std::array<int, 5> supportedObjectTypes{VTK_RECTILINEAR_GRID, VTK_STRUCTURED_GRID,
                                                  VTK_IMAGE_DATA, VTK_UNIFORM_GRID,
                                                  VTK_STRUCTURED_POINTS};

namespace detail {

std::shared_ptr<TensorField2D> vtkToTensorField(size3_t dimensions, vtkDataSet* vtkData,
                                                int arrayIndex) {
    auto array = vtkData->GetPointData()->GetArray(arrayIndex);
    const int numComponents = array->GetNumberOfComponents();

    size2_t dims2D{dimensions};
    dvec2 extent{0.0};
    dvec2 offset{0.0};

    std::vector<dmat2> data;
    data.reserve(glm::compMul(dimensions));

    if (numComponents == 4) {
        auto worker = [&]<typename T>(T* typedArray) {
            const auto nTuples = array->GetNumberOfTuples();
            if (static_cast<vtkIdType>(glm::compMul(dimensions)) != nTuples) {
                throw Exception("Invalid dimensions");
            }

            for (vtkIdType id = 0; id < nTuples; ++id) {
                data.emplace_back(
                    typedArray->GetTypedComponent(id, 0), typedArray->GetTypedComponent(id, 1),
                    typedArray->GetTypedComponent(id, 2), typedArray->GetTypedComponent(id, 3));
            }
        };
        if (!vtkArrayDispatch::Dispatch::Execute(array, worker)) {
            throw Exception("no matching type");
        }
    } else if (numComponents == 9) {

        // Project tensor onto plane with extent 1 to yield 2x2 tensors (mat2)
        CartesianCoordinateAxis axis;
        if (dimensions.z == 1) {
            axis = CartesianCoordinateAxis::Z;
        } else if (dimensions.y == 1) {
            axis = CartesianCoordinateAxis::Y;
        } else {
            axis = CartesianCoordinateAxis::X;
        }

        auto project2D = [&](CartesianCoordinateAxis axis) {
            const dmat4 m{*vtk::getModelMatrix(vtkData)};
            int dstIndex = 0;
            for (int i = 0; i < 3; ++i) {
                if (static_cast<CartesianCoordinateAxis>(i) == axis) continue;
                dims2D[dstIndex] = dimensions[i];
                offset[dstIndex] = m[3][i];
                extent[dstIndex] = glm::length(dvec3(m[i]));
                ++dstIndex;
            }
        };
        project2D(axis);

        std::vector<dmat3> tensors3D;
        tensors3D.reserve(glm::compMul(dimensions));

        auto worker = [&]<typename T>(T* typedArray) {
            const auto nTuples = array->GetNumberOfTuples();
            if (static_cast<vtkIdType>(glm::compMul(dimensions)) != nTuples) {
                throw Exception("Invalid dimensions");
            }

            for (vtkIdType id = 0; id < nTuples; ++id) {
                tensors3D.emplace_back(
                    typedArray->GetTypedComponent(id, 0), typedArray->GetTypedComponent(id, 1),
                    typedArray->GetTypedComponent(id, 2),

                    typedArray->GetTypedComponent(id, 3), typedArray->GetTypedComponent(id, 4),
                    typedArray->GetTypedComponent(id, 5),

                    typedArray->GetTypedComponent(id, 6), typedArray->GetTypedComponent(id, 7),
                    typedArray->GetTypedComponent(id, 8));
            }
        };
        if (!vtkArrayDispatch::Dispatch::Execute(array, worker)) {
            throw Exception("no matching type");
        }

        std::transform(
            tensors3D.begin(), tensors3D.end(), std::back_inserter(data),
            [&](const dmat3& tensor) { return tensorutil::getProjectedTensor(tensor, axis); });
    } else {
        throw Exception(IVW_CONTEXT_CUSTOM("VTKDataSetToTensorField2D::vtkToTensorField"),
                        "unsupported number of components for tensor data ({}), expected 4 or 9",
                        numComponents);
    }

    std::shared_ptr<TensorField2D> tensorField =
        std::make_shared<TensorField2D>(dims2D, data, extent);
    tensorField->setOffset(offset);
    return tensorField;
}

}  // namespace detail

VTKToTensorField2D::VTKToTensorField2D()
    : Processor{}
    , inport_{"vtkDataSetInport", "vtkDataSet",
              "VTK dataset (VTK_RECTILINEAR_GRID, VTK_STRUCTURED_GRID, VTK_IMAGE_DATA, "
              "VTK_UNIFORM_GRID, VTK_STRUCTURED_POINTS)"_help}
    , outport_{"tensorField2DOutport", "Resulting 2D Tensor field"_help}
    , sourceTensors_{"tensors", "Tensors"} {

    addPorts(inport_, outport_);

    addProperties(sourceTensors_);
}

void VTKToTensorField2D::updateSources(vtkDataSet* data) {
    std::vector<OptionPropertyOption<int>> opts;

    if (data) {
        auto arrays = data->GetPointData();
        const auto nArrays = arrays->GetNumberOfArrays();
        for (int i = 0; i < nArrays; ++i) {
            auto array = arrays->GetAbstractArray(i);
            opts.emplace_back(
                util::stripIdentifier(array->GetName()),
                fmt::format("{} ({}D {})", array->GetName(), array->GetNumberOfComponents(),
                            array->GetDataTypeAsString()),
                i);
        }
    }

    sourceTensors_.replaceOptions(opts);
}

void VTKToTensorField2D::process() {
    auto data = inport_.getData();
    auto vtkData = vtkDataSet::SafeDownCast(data);

    if (inport_.isChanged()) {
        updateSources(vtkData);
    }

    if (sourceTensors_.size() == 0) {
        outport_.detachData();
        return;
    }

    if (int dot = vtkData->GetDataObjectType(); !util::contains(supportedObjectTypes, dot)) {
        throw Exception(IVW_CONTEXT, "unsupported vtkDataSet {} ({})",
                        vtkDataObjectTypes::GetClassNameFromTypeId(dot), dot);
    }

    if (inport_.isChanged() || sourceTensors_.isModified()) {
        const ivec3 dims{*vtk::getDimensions(vtkData)};

        if (glm::compMin(dims) > 1) {
            throw Exception(IVW_CONTEXT, "VTK dataset is three dimensional {}, expected 2D input",
                            dims);
        }

        tensorField_ = detail::vtkToTensorField(dims, vtkData, sourceTensors_.getSelectedValue());
    }

    outport_.setData(tensorField_);
}

}  // namespace inviwo
