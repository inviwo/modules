/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2024-2025 Inviwo Foundation
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

#include <inviwo/vtk/util/vtkdatautils.h>
#include <inviwo/core/util/glm.h>

#include <vtkDataSet.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredPoints.h>
#include <vtkImageData.h>
#include <vtkMatrix3x3.h>

namespace inviwo {

namespace vtk {

std::optional<ivec3> getDimensions(vtkDataSet* vtkData) {
    switch (vtkData->GetDataObjectType()) {
        case VTK_RECTILINEAR_GRID: {
            ivec3 dims{0};
            vtkRectilinearGrid::SafeDownCast(vtkData)->GetDimensions(glm::value_ptr(dims));
            return dims;
        }
        case VTK_STRUCTURED_GRID: {
            ivec3 dims{0};
            vtkStructuredGrid::SafeDownCast(vtkData)->GetDimensions(glm::value_ptr(dims));
            return dims;
        }
        case VTK_IMAGE_DATA:
        case VTK_UNIFORM_GRID:
        case VTK_STRUCTURED_POINTS: {
            ivec3 dims{0};
            vtkImageData::SafeDownCast(vtkData)->GetDimensions(glm::value_ptr(dims));
            return dims;
        }
        default:
            break;
    }
    return std::nullopt;
}

std::optional<dmat4> getModelMatrix(vtkDataSet* vtkData) {
    switch (vtkData->GetDataObjectType()) {
        case VTK_RECTILINEAR_GRID:
        case VTK_STRUCTURED_GRID: {
            vtkData->ComputeBounds();
            dvec3 dataMin{0.0};
            dvec3 dataMax{0.0};
            if (vtkData) {
                vtkData->ComputeBounds();
                auto bounds = vtkData->GetBounds();
                dataMin = dvec3{bounds[0], bounds[2], bounds[4]};
                dataMax = dvec3{bounds[1], bounds[3], bounds[5]};
            }

            auto m = glm::scale(dataMax - dataMin);
            m[3] = dvec4(dataMin, 1.0);
            return m;
        }
        case VTK_IMAGE_DATA:
        case VTK_UNIFORM_GRID:
        case VTK_STRUCTURED_POINTS: {
            auto vtkImage = vtkImageData::SafeDownCast(vtkData);
            const dvec3 offset = glm::make_vec3(vtkImage->GetOrigin());
            const dvec3 spacing = glm::make_vec3(vtkImage->GetSpacing());
            const dmat3 direction = glm::make_mat3x3(vtkImage->GetDirectionMatrix()->GetData());
            const dvec3 ddim{getDimensions(vtkImage).value_or(ivec3(1))};
            return dmat4{dvec4{direction[0] * ddim[0] * spacing[0], 0.0},
                         dvec4{direction[1] * ddim[1] * spacing[1], 0.0},
                         dvec4{direction[2] * ddim[2] * spacing[2], 0.0}, dvec4{offset, 1.0}};
        }
        default:
            break;
    }
    return std::nullopt;
}

}  // namespace vtk

}  // namespace inviwo
