/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2022-2025 Inviwo Foundation
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

#include <inviwo/vtk/processors/volumetovtk.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>

#include <inviwo/core/util/glm.h>
#include <inviwo/core/util/zip.h>
#include <inviwo/core/util/exception.h>

#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkInformation.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VolumeToVTK::processorInfo_{
    "org.inviwo.VolumeToVTK",                // Class identifier
    "Volume To VTK",                         // Display name
    "VTK",                                   // Category
    CodeState::Experimental,                 // Code state
    Tags::CPU | Tag{"VTK"} | Tag{"Volume"},  // Tags
    R"(Creates a vtkImageData dataset from a volume. The volume from the optional 
       inport are included as additional arrays. The volumes are required to have 
       the same dimensions.)"_unindentHelp,
};
const ProcessorInfo& VolumeToVTK::getProcessorInfo() const { return processorInfo_; }

VolumeToVTK::VolumeToVTK()
    : Processor()
    , inport_("inport")
    , optionalVolumes_("volumes")
    , outport_("outport", "vtkImageData")
    , space_{
          "space", "Space", {{"data", "Data", Space::Data}, {"model", "Model", Space::Model}}, 1} {
    addPorts(inport_, optionalVolumes_, outport_);
    optionalVolumes_.setOptional(true);
    addProperty(space_);
}

void VolumeToVTK::process() {
    auto volume = inport_.getData();

    for (auto&& [index, v] : util::enumerate(optionalVolumes_.getVectorData())) {
        if (volume->getDimensions() != v->getDimensions()) {
            throw Exception(IVW_CONTEXT,
                            "Volume dimensions must match, expected {} and got {} from input {}",
                            volume->getDimensions(), v->getDimensions(), index);
        }
    }

    const auto dim = static_cast<ivec3>(volume->getDimensions());
    const dvec3 ddim{dim};
    data_ = vtkSmartPointer<vtkImageData>::New();
    data_->SetDimensions(dim.x, dim.y, dim.z);

    switch (space_.getSelectedValue()) {
        case Space::Data: {
            const dvec3 spacing{dvec3{1.0} / ddim};
            const dvec3 offset{0.0};
            const dmat3 direction{1.0};
            data_->SetSpacing(spacing.x, spacing.y, spacing.z);
            data_->SetOrigin(glm::value_ptr(offset));
            data_->SetDirectionMatrix(glm::value_ptr(direction));
            break;
        }
        case Space::Model:
            const dvec3 offset = volume->getOffset();
            const dmat3 basis = volume->getBasis();
            const dvec3 length{glm::length(basis[0]), glm::length(basis[1]), glm::length(basis[2])};
            const dvec3 spacing{length / ddim};
            const dmat3 direction{basis[0] / length[0], basis[1] / length[1], basis[2] / length[2]};
            data_->SetSpacing(spacing.x, spacing.y, spacing.z);
            data_->SetOrigin(glm::value_ptr(offset));
            data_->SetDirectionMatrix(glm::value_ptr(direction));
            break;
    }

    const auto vtkType = [&](DataFormatId formatId) {
        switch (formatId) {
            case DataFormatId::Int8:
            case DataFormatId::Vec2Int8:
            case DataFormatId::Vec3Int8:
            case DataFormatId::Vec4Int8: {
                return VTK_CHAR;
            }
            case DataFormatId::UInt8:
                [[fallthrough]];
            case DataFormatId::Vec2UInt8:
                [[fallthrough]];
            case DataFormatId::Vec3UInt8:
                [[fallthrough]];
            case DataFormatId::Vec4UInt8: {
                return VTK_UNSIGNED_CHAR;
            }
            case DataFormatId::Int16:
                [[fallthrough]];
            case DataFormatId::Vec2Int16:
                [[fallthrough]];
            case DataFormatId::Vec3Int16:
                [[fallthrough]];
            case DataFormatId::Vec4Int16: {
                return VTK_SHORT;
            }
            case DataFormatId::UInt16:
                [[fallthrough]];
            case DataFormatId::Vec2UInt16:
                [[fallthrough]];
            case DataFormatId::Vec3UInt16:
                [[fallthrough]];
            case DataFormatId::Vec4UInt16: {
                return VTK_UNSIGNED_SHORT;
            }
            case DataFormatId::Int32:
                [[fallthrough]];
            case DataFormatId::Vec2Int32:
                [[fallthrough]];
            case DataFormatId::Vec3Int32:
                [[fallthrough]];
            case DataFormatId::Vec4Int32: {
                return VTK_INT;
            }
            case DataFormatId::UInt32:
                [[fallthrough]];
            case DataFormatId::Vec2UInt32:
                [[fallthrough]];
            case DataFormatId::Vec3UInt32:
                [[fallthrough]];
            case DataFormatId::Vec4UInt32: {
                return VTK_UNSIGNED_INT;
            }
            case DataFormatId::Int64:
                [[fallthrough]];
            case DataFormatId::Vec2Int64:
                [[fallthrough]];
            case DataFormatId::Vec3Int64:
                [[fallthrough]];
            case DataFormatId::Vec4Int64: {
                return VTK_LONG;
            }
            case DataFormatId::UInt64:
                [[fallthrough]];
            case DataFormatId::Vec2UInt64:
                [[fallthrough]];
            case DataFormatId::Vec3UInt64:
                [[fallthrough]];
            case DataFormatId::Vec4UInt64: {
                return VTK_UNSIGNED_LONG;
            }
            case DataFormatId::Float32:
                [[fallthrough]];
            case DataFormatId::Vec2Float32:
                [[fallthrough]];
            case DataFormatId::Vec3Float32:
                [[fallthrough]];
            case DataFormatId::Vec4Float32: {
                return VTK_FLOAT;
            }
            case DataFormatId::Float64:
                [[fallthrough]];
            case DataFormatId::Vec2Float64:
                [[fallthrough]];
            case DataFormatId::Vec3Float64:
                [[fallthrough]];
            case DataFormatId::Vec4Float64: {
                return VTK_DOUBLE;
            }
            default: {
                throw Exception("Cannot map type to VTK.", IVW_CONTEXT_CUSTOM("VolumeToVtk"));
            }
        }
    };

    const auto nComp = volume->getDataFormat()->getComponents();
    data_->AllocateScalars(vtkType(volume->getDataFormat()->getId()), static_cast<int>(nComp));
    void* dst = data_->GetScalarPointer();
    const void* src = volume->getRepresentation<VolumeRAM>()->getData();
    std::memcpy(dst, src, glm::compMul(dim) * volume->getDataFormat()->getSizeInBytes());

    volumeData_.clear();
    for (auto&& [outport, v] : optionalVolumes_.getSourceVectorData()) {
        auto* dataFormat = v->getDataFormat();

        auto scalars = vtkSmartPointer<vtkDataArray>(
            vtkDataArray::CreateDataArray(vtkType(dataFormat->getId())));
        scalars->SetNumberOfComponents(static_cast<int>(dataFormat->getComponents()));
        scalars->SetName(outport->getProcessor()->getIdentifier().c_str());
        scalars->SetNumberOfTuples(glm::compMul(dim));

        void* dstData = scalars->GetVoidPointer(0);
        const void* srcData = v->getRepresentation<VolumeRAM>()->getData();
        std::memcpy(dstData, srcData, glm::compMul(dim) * dataFormat->getSizeInBytes());

        data_->GetPointData()->AddArray(scalars);
        volumeData_.push_back(scalars);
    }

    vtkInformation* info = data_->GetPointData()->GetScalars()->GetInformation();
    info->Set(vtkDataArray::UNITS_LABEL(), fmt::format("{}{: [}", volume->dataMap.valueAxis.name,
                                                       volume->dataMap.valueAxis.unit));
    std::array<double, 8> range;
    for (size_t i = 0; i < nComp; ++i) {
        range[i * 2] = volume->dataMap.dataRange[0];
        range[i * 2 + 1] = volume->dataMap.dataRange[1];
    }
    info->Set(vtkDataArray::COMPONENT_RANGE(), range.data(), static_cast<int>(nComp * 2));

    outport_.setData(data_);
}
}  // namespace inviwo
