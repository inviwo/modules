/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2025 Inviwo Foundation
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

#pragma once

#include <modules/opengl/shader/shaderutils.h>
#include <inviwo/tensorvisbase/datastructures/tensorfield2d.h>
#include <inviwo/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/tensorvisbase/datastructures/tensorfield3d.h>
#include <inviwo/tensorvisbase/algorithm/tensorfieldsampling.h>
#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <Eigen/Dense>
#include <modules/eigenutils/eigenutils.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/util/settings/systemsettings.h>
#include <inviwo/core/datastructures/geometry/geometrytype.h>
#include <inviwo/core/util/volumeramutils.h>
#include <inviwo/core/util/imageramutils.h>

namespace inviwo::tensorutil {

IVW_MODULE_TENSORVISBASE_API void bindTensorFieldAsColorTexture(
    std::shared_ptr<Image>& texture, std::shared_ptr<const TensorField2D> tensorField,
    Shader& shader, TextureUnitContainer& textureUnits);

IVW_MODULE_TENSORVISBASE_API void bindTensorFieldAsColorTexture(std::shared_ptr<Image>& texture,
                                                                TensorField2DInport& inport,
                                                                Shader& shader,
                                                                TextureUnitContainer& textureUnits);

IVW_MODULE_TENSORVISBASE_API void bindTensorFieldAsColorTextures(
    std::shared_ptr<const TensorField3D>& tensorField, Shader* shader,
    TextureUnitContainer& textureUnits);

std::shared_ptr<TensorField2D> IVW_MODULE_TENSORVISBASE_API
subsample2D(std::shared_ptr<const TensorField2D> tensorField, size2_t newDimensions,
            const InterpolationMethod method = InterpolationMethod::Barycentric);

IVW_MODULE_TENSORVISBASE_API std::shared_ptr<TensorField3D> subsample3D(
    std::shared_ptr<const TensorField3D> tensorField, size3_t newDimensions,
    const InterpolationMethod method = InterpolationMethod::Linear);

IVW_MODULE_TENSORVISBASE_API std::shared_ptr<TensorField3D> subsample3D(
    std::shared_ptr<const TensorField3D> tensorField, size3_t newDimensions,
    const InterpolationMethod method, std::function<void(float)> fun);

IVW_MODULE_TENSORVISBASE_API std::shared_ptr<PosTexColorMesh>
generateBoundingBoxAdjacencyForTensorField(std::shared_ptr<const TensorField3D> tensorField,
                                           const vec4 color);

IVW_MODULE_TENSORVISBASE_API std::shared_ptr<BasicMesh> generateBoundingBoxForTensorField(
    std::shared_ptr<const TensorField3D> tensorField, const vec4 color);

IVW_MODULE_TENSORVISBASE_API std::shared_ptr<BasicMesh> generateSliceLevelGeometryForTensorField(
    std::shared_ptr<const TensorField3D> tensorField, const vec4 color,
    const CartesianCoordinateAxis axis, const size_t sliceNr);

IVW_MODULE_TENSORVISBASE_API std::shared_ptr<BasicMesh> generateSlicePlaneGeometryForTensorField(
    std::shared_ptr<const TensorField3D> tensorField, const vec4 color,
    const CartesianCoordinateAxis axis, const size_t sliceNr);

namespace detail {
std::shared_ptr<BasicMesh> generateSliceGeometry(std::shared_ptr<const TensorField3D> tensorField,
                                                 const vec4 color,
                                                 const CartesianCoordinateAxis axis,
                                                 const size_t sliceNr);
}

template <typename C>
void forEachVoxel(const TensorField3D& v, C callback) {
    util::forEachVoxel(v.getDimensions(), callback);
}

template <typename C>
void forEachVoxelParallel(const TensorField3D& v, C callback, size_t jobs = 0) {
    util::forEachVoxelParallel(v.getDimensions(), callback, jobs);
}

template <typename C>
void forEachFixel(const TensorField2D& v, C callback) {
    util::forEachPixel(v.getDimensions(), callback);
}

template <typename C>
void forEachFixelParallel(const TensorField2D& v, C callback, size_t jobs = 0) {
    util::forEachPixelParallel(v.getDimensions(), callback, jobs);
}

}  // namespace inviwo::tensorutil
