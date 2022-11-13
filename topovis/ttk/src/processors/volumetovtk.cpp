/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2022 Inviwo Foundation
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

#include <inviwo/ttk/processors/volumetovtk.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>

#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkInformation.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VolumeToVTK::processorInfo_{
    "org.inviwo.VolumeToVTK",              // Class identifier
    "Volume To VTK",                       // Display name
    "VTK",                                 // Category
    CodeState::Experimental,               // Code state
    Tag::CPU | Tag{"VTK"} | Tag{"Volume"}  // Tags
};
const ProcessorInfo VolumeToVTK::getProcessorInfo() const { return processorInfo_; }

VolumeToVTK::VolumeToVTK() : Processor(), inport_("inport"), outport_("outport", "vtkImageData") {
    addPorts(inport_, outport_);
}

void VolumeToVTK::process() {
    auto volume = inport_.getData();

    const auto dim = static_cast<ivec3>(volume->getDimensions());
    const dvec3 offset = volume->getOffset();
    const dmat3 basis = volume->getBasis();

    data_ = vtkSmartPointer<vtkImageData>::New();
    data_->SetDimensions(dim.x, dim.y, dim.z);
    data_->SetOrigin(glm::value_ptr(offset));
    data_->SetDirectionMatrix(glm::value_ptr(basis));

    const auto vtkType = [&]() {
        switch (volume->getDataFormat()->getId()) {
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
    }();

    const auto nComp = volume->getDataFormat()->getComponents();
    data_->AllocateScalars(vtkType, static_cast<int>(nComp));
    void* dst = data_->GetScalarPointer();
    const void* src = volume->getRepresentation<VolumeRAM>()->getData();
    std::memcpy(dst, src, glm::compMul(dim) * volume->getDataFormat()->getSize());

    vtkInformation* info = data_->GetPointData()->GetScalars()->GetInformation();
    info->Set(vtkDataArray::UNITS_LABEL(), fmt::format("{}{: [}", volume->dataMap_.valueAxis.name,
                                                       volume->dataMap_.valueAxis.unit));
    std::array<double, 8> range;
    for (size_t i = 0; i < nComp; ++i) {
        range[i * 2] = volume->dataMap_.dataRange[0];
        range[i * 2 + 1] = volume->dataMap_.dataRange[1];
    }
    info->Set(vtkDataArray::COMPONENT_RANGE(), range.data(), static_cast<int>(nComp * 2));

    outport_.setData(data_);
}
}  // namespace inviwo
