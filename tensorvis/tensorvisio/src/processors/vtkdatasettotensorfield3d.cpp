/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2024 Inviwo Foundation
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

#include <inviwo/tensorvisio/processors/vtkdatasettotensorfield3d.h>
#include <inviwo/core/network/networklock.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/core/util/utilities.h>
#include <inviwo/core/util/glm.h>
#include <inviwo/core/util/formats.h>
#include <inviwo/core/util/exception.h>

#include <inviwo/vtk/util/arrayutils.h>
#include <inviwo/vtk/util/vtkdatautils.h>

#include <fmt/core.h>

#include <vtkDataSet.h>
#include <vtkPointData.h>
#include <vtkMatrix3x3.h>
#include <vtkAbstractArray.h>
#include <vtkArrayDispatch.h>
#include <vtkGenericDataArray.h>
#include <vtkDataObjectTypes.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKDataSetToTensorField3D::processorInfo_{
    "org.inviwo.VTKDataSetToTensorField3D",  // Class identifier
    "VTK Data Set To Tensor Field 3D",       // Display name
    "VTK",                                   // Category
    CodeState::Experimental,                 // Code state
    Tags::CPU | Tag{"VTK"} | Tag{"Tensor"},  // Tags
    R"(Converts a VTK data set to a 3D tensor field. Unstructured grids are not supported.)"_unindentHelp,
};
const ProcessorInfo VTKDataSetToTensorField3D::getProcessorInfo() const { return processorInfo_; }

constexpr std::array<int, 5> supportedObjectTypes{VTK_RECTILINEAR_GRID, VTK_STRUCTURED_GRID,
                                                  VTK_IMAGE_DATA, VTK_UNIFORM_GRID,
                                                  VTK_STRUCTURED_POINTS};

namespace detail {

std::shared_ptr<TensorField3D> vtkToTensorField(size3_t dimensions, vtkDataSet* vtkData,
                                                int arrayIndex) {
    auto array = vtkData->GetPointData()->GetArray(arrayIndex);
    if (array->GetNumberOfComponents() != 9) {
        throw Exception(IVW_CONTEXT_CUSTOM("VTKDataSetToTensorField3D::vtkToTensorField"),
                        "unsupported number of components for tensor data ({}), expected 9",
                        array->GetNumberOfComponents());
    }

    std::shared_ptr<TensorField3D> tensorfield;

    auto worker = [&]<typename T>(T* typedArray) {
        const auto nTuples = array->GetNumberOfTuples();

        if (static_cast<vtkIdType>(glm::compMul(dimensions)) != nTuples) {
            throw Exception("Invalid dimensions");
        }

        std::vector<dmat3> data;
        data.reserve(glm::compMul(dimensions));

        for (vtkIdType id = 0; id < nTuples; ++id) {
            data.emplace_back(
                typedArray->GetTypedComponent(id, 0), typedArray->GetTypedComponent(id, 1),
                typedArray->GetTypedComponent(id, 2),

                typedArray->GetTypedComponent(id, 3), typedArray->GetTypedComponent(id, 4),
                typedArray->GetTypedComponent(id, 5),

                typedArray->GetTypedComponent(id, 6), typedArray->GetTypedComponent(id, 7),
                typedArray->GetTypedComponent(id, 8));
        }

        tensorfield = std::make_shared<TensorField3D>(dimensions, data);
    };

    if (!vtkArrayDispatch::Dispatch::Execute(array, worker)) {
        throw Exception("no matching type");
    }

    return tensorfield;
}

std::vector<double> vtkToScalars(vtkDataSet* vtkData, int arrayIndex) {
    auto array = vtkData->GetPointData()->GetArray(arrayIndex);

    if (array->GetNumberOfComponents() != 1) {
        throw Exception(IVW_CONTEXT_CUSTOM("VTKDataSetToTensorField3D::vtkToScalars"),
                        "unsupported number of components for scalar data ({}), expected 1",
                        array->GetNumberOfComponents());
    }
    const auto nTuples = array->GetNumberOfTuples();
    std::vector<double> scalars(nTuples);

    auto worker = [&]<typename T>(T* typedArray) {
        for (vtkIdType i = 0; i < nTuples; ++i) {
            const auto val = std::invoke([](auto item) { return static_cast<double>(item); },
                                         vtk::getTuple<1>(typedArray, i));
            scalars[i] = val;
        }
    };

    if (!vtkArrayDispatch::Dispatch::Execute(array, worker)) {
        throw Exception("no matching type for scalar values");
    }

    return scalars;
}

}  // namespace detail

VTKDataSetToTensorField3D::VTKDataSetToTensorField3D()
    : Processor{}
    , inport_{"dataSetInport", "vtkDataSet",
              "VTK dataset (VTK_RECTILINEAR_GRID, VTK_STRUCTURED_GRID, VTK_IMAGE_DATA, "
              "VTK_UNIFORM_GRID, VTK_STRUCTURED_POINTS)"_help}
    , outport_{"tensorField3DOutport"}
    , sourceTensors_{"tensors", "Tensors"}
    , sourceScalars_{"scalars", "Scalars"}
    , basis_{"Basis", "Basis and Offset"}
    , normalizeExtents_{"normalizeExtents", "Normalize extents", false} {

    addPorts(inport_, outport_);
    addProperties(sourceTensors_, sourceScalars_, normalizeExtents_, basis_);
}

void VTKDataSetToTensorField3D::updateSources(vtkDataSet* data) {
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

    opts.emplace_back("none", "None", -1);
    sourceScalars_.replaceOptions(opts);
}

void VTKDataSetToTensorField3D::process() {
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

    if (inport_.isChanged() || sourceTensors_.isModified() || sourceScalars_.isModified()) {
        const ivec3 dims{*vtk::getDimensions(vtkData)};

        tensorField_ = detail::vtkToTensorField(dims, vtkData, sourceTensors_.getSelectedValue());
        tensorField_->setModelMatrix(*vtk::getModelMatrix(vtkData));

        if (normalizeExtents_) {
            dvec3 extent{tensorField_->getExtent()};
            extent /= glm::compMax(extent);
            tensorField_->setBasis(dmat3(glm::scale(extent)));
        }

        if (sourceScalars_.getSelectedValue() >= 0) {
            tensorField_->addMetaData<tensor::HillYieldCriterion>(
                detail::vtkToScalars(vtkData, sourceScalars_.getSelectedValue()),
                TensorFeature::HillYieldCriterion);
        }

        const bool deserializing = getNetwork()->isDeserializing();
        basis_.updateForNewEntity(*tensorField_, deserializing);
    }

    basis_.updateEntity(*tensorField_);

    outport_.setData(tensorField_);
}

}  // namespace inviwo
