/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2023-2025 Inviwo Foundation
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

#include <inviwo/vtk/util/arrayutils.h>

#include <inviwo/core/datastructures/image/layer.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/util/glm.h>

#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkInformation.h>
#include <vtkMatrix3x3.h>

#include <regex>

namespace inviwo::vtk {

std::shared_ptr<Layer> vtkImageDataToLayer(vtkImageData* vtkImage, int arrayIndex, int precision) {
    if (!vtkImage) return nullptr;

    const ivec2 dim = glm::make_vec2(vtkImage->GetDimensions());
    auto array = vtkImage->GetPointData()->GetArray(arrayIndex);

    auto ram = [&]() {
        if (precision == 8) {
            return vtk::arrayToLayerRAM(static_cast<size2_t>(dim), array, [](auto item) {
                using Dst = decltype(changePrecision<8>(item));
                return static_cast<Dst>(item);
            });
        } else if (precision == 16) {
            return vtk::arrayToLayerRAM(static_cast<size2_t>(dim), array, [](auto item) {
                using Dst = decltype(changePrecision<16>(item));
                return static_cast<Dst>(item);
            });
        } else if (precision == 32) {
            return vtk::arrayToLayerRAM(static_cast<size2_t>(dim), array, [](auto item) {
                using Dst = decltype(changePrecision<32>(item));
                return static_cast<Dst>(item);
            });
        } else if (precision == 64) {
            return vtk::arrayToLayerRAM(static_cast<size2_t>(dim), array, [](auto item) {
                using Dst = decltype(changePrecision<64>(item));
                return static_cast<Dst>(item);
            });
        } else {
            return vtk::arrayToLayerRAM(static_cast<size2_t>(dim), array,
                                        [](auto item) { return item; });
        }
    }();

    if (ram->getDataFormatId() == DataFormatId::NotSpecialized) {
        throw Exception(IVW_CONTEXT_CUSTOM("vtk::vtkImageDataToLayer"),
                        "No valid inviwo DataFormat found for '{}' with '{}' components",
                        array->GetDataTypeAsString(), array->GetNumberOfComponents());
    }

    auto layer = std::make_shared<Layer>(ram);

    const dvec3 offset = glm::make_vec3(vtkImage->GetOrigin());
    const dvec3 spacing = glm::make_vec3(vtkImage->GetSpacing());
    const dmat3 direction = glm::make_mat3x3(vtkImage->GetDirectionMatrix()->GetData());
    const dvec3 ddim{dim, 0};
    const dmat3 basis{direction[0] * ddim[0] * spacing[0], direction[1] * ddim[1] * spacing[1],
                      direction[2] * ddim[2] * spacing[2]};

    layer->setSwizzleMask(swizzlemasks::defaultData(array->GetNumberOfComponents()));

    layer->setBasis(static_cast<mat3>(basis));
    layer->setOffset(static_cast<vec3>(offset));

    vtkInformation* info = array->GetInformation();
    if (info->Has(vtkDataArray::UNITS_LABEL())) {
        std::regex re{R"((.*)\[(.*)\])"};
        std::smatch m;
        auto label = std::string{info->Get(vtkDataArray::UNITS_LABEL())};
        if (std::regex_match(label, m, re)) {
            layer->dataMap.valueAxis.name = util::trim(m.str(1));
            layer->dataMap.valueAxis.unit = units::unit_from_string(m.str(2));
        }
    }
    if (layer->dataMap.valueAxis.name.empty()) {
        layer->dataMap.valueAxis.name = array->GetName();
    }

    if (info->Has(vtkDataArray::COMPONENT_RANGE())) {
        auto* range = info->Get(vtkDataArray::COMPONENT_RANGE());
        layer->dataMap.dataRange[0] = range[0];
        layer->dataMap.dataRange[1] = range[1];
        layer->dataMap.valueRange = layer->dataMap.dataRange;
    } else {
        auto* range = array->GetRange();
        layer->dataMap.dataRange[0] = range[0];
        layer->dataMap.dataRange[1] = range[1];
        layer->dataMap.valueRange = layer->dataMap.dataRange;
    }

    return layer;
}

std::shared_ptr<Volume> vtkImageDataToVolume(vtkImageData* vtkImage, int arrayIndex,
                                             int precision) {
    if (!vtkImage) return nullptr;

    const ivec3 dim = glm::make_vec3(vtkImage->GetDimensions());
    auto array = vtkImage->GetPointData()->GetArray(arrayIndex);

    auto ram = [&]() {
        if (precision == 8) {
            return vtk::arrayToVolumeRAM(static_cast<size3_t>(dim), array, [](auto item) {
                using Dst = decltype(vtk::changePrecision<8>(item));
                return static_cast<Dst>(item);
            });
        } else if (precision == 16) {
            return vtk::arrayToVolumeRAM(static_cast<size3_t>(dim), array, [](auto item) {
                using Dst = decltype(vtk::changePrecision<16>(item));
                return static_cast<Dst>(item);
            });
        } else if (precision == 32) {
            return vtk::arrayToVolumeRAM(static_cast<size3_t>(dim), array, [](auto item) {
                using Dst = decltype(vtk::changePrecision<32>(item));
                return static_cast<Dst>(item);
            });
        } else if (precision == 64) {
            return vtk::arrayToVolumeRAM(static_cast<size3_t>(dim), array, [](auto item) {
                using Dst = decltype(vtk::changePrecision<64>(item));
                return static_cast<Dst>(item);
            });
        } else {
            return vtk::arrayToVolumeRAM(static_cast<size3_t>(dim), array,
                                         [](auto item) { return item; });
        }
    }();

    if (ram->getDataFormatId() == DataFormatId::NotSpecialized) {
        throw Exception(IVW_CONTEXT_CUSTOM("vtk::vtkImageDataToVolume"),
                        "No valid inviwo DataFormat found for '{}' with '{}' components",
                        array->GetDataTypeAsString(), array->GetNumberOfComponents());
    }

    auto volume = std::make_shared<Volume>(ram);

    const dvec3 offset = glm::make_vec3(vtkImage->GetOrigin());
    const dvec3 spacing = glm::make_vec3(vtkImage->GetSpacing());
    const dmat3 direction = glm::make_mat3x3(vtkImage->GetDirectionMatrix()->GetData());
    const dvec3 ddim{dim};
    const dmat3 basis{direction[0] * ddim[0] * spacing[0], direction[1] * ddim[1] * spacing[1],
                      direction[2] * ddim[2] * spacing[2]};

    volume->setBasis(static_cast<mat3>(basis));
    volume->setOffset(static_cast<vec3>(offset));

    vtkInformation* info = array->GetInformation();
    if (info->Has(vtkDataArray::UNITS_LABEL())) {
        std::regex re{R"((.*)\[(.*)\])"};
        std::smatch m;
        auto label = std::string{info->Get(vtkDataArray::UNITS_LABEL())};
        if (std::regex_match(label, m, re)) {
            volume->dataMap.valueAxis.name = util::trim(m.str(1));
            volume->dataMap.valueAxis.unit = units::unit_from_string(m.str(2));
        }
    }
    if (volume->dataMap.valueAxis.name.empty()) {
        volume->dataMap.valueAxis.name = array->GetName();
    }

    if (info->Has(vtkDataArray::COMPONENT_RANGE())) {
        auto* range = info->Get(vtkDataArray::COMPONENT_RANGE());
        volume->dataMap.dataRange[0] = range[0];
        volume->dataMap.dataRange[1] = range[1];
        volume->dataMap.valueRange = volume->dataMap.dataRange;
    } else {
        auto* range = array->GetRange();
        volume->dataMap.dataRange[0] = range[0];
        volume->dataMap.dataRange[1] = range[1];
        volume->dataMap.valueRange = volume->dataMap.dataRange;
    }

    return volume;
}

}  // namespace inviwo::vtk
