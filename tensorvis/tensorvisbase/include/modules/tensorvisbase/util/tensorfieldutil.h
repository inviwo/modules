/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2018 Inviwo Foundation
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
#include <modules/tensorvisbase/datastructures/tensorfield2d.h>
#include <modules/tensorvisbase/ports/tensorfieldport.h>
#include <modules/tensorvisbase/datastructures/tensorfield3d.h>
#include <modules/tensorvisbase/tensorvisbasemoduledefine.h>
#include <Eigen/Dense>
#include <modules/eigenutils/eigenutils.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/util/settings/systemsettings.h>
#include <inviwo/core/datastructures/geometry/geometrytype.h>

namespace inviwo {
namespace tensorutil {

void IVW_MODULE_TENSORVISBASE_API bindTensorFieldAsColorTexture(
    std::shared_ptr<Image> &texture, std::shared_ptr<const TensorField2D> tensorField,
    Shader &shader, TextureUnitContainer &textureUnits);

void IVW_MODULE_TENSORVISBASE_API
bindTensorFieldAsColorTexture(std::shared_ptr<Image> &texture, TensorField2DInport &inport,
                              Shader &shader, TextureUnitContainer &textureUnits);

void IVW_MODULE_TENSORVISBASE_API
bindTensorFieldAsColorTextures(std::shared_ptr<const TensorField3D> &tensorField, Shader *shader,
                               TextureUnitContainer &textureUnits);

std::shared_ptr<TensorField2D> IVW_MODULE_TENSORVISBASE_API
getSlice2D(std::shared_ptr<const TensorField3D> inTensorField, const CartesianCoordinateAxis axis,
           const size_t sliceNumber);

std::shared_ptr<TensorField3D> IVW_MODULE_TENSORVISBASE_API
getSlice3D(std::shared_ptr<const TensorField3D> inTensorField, const CartesianCoordinateAxis axis,
           const size_t sliceNumber);

std::shared_ptr<TensorField2D> IVW_MODULE_TENSORVISBASE_API
subsample2D(std::shared_ptr<const TensorField2D> tensorField, size2_t newDimensions,
            const InterpolationMethod method = InterpolationMethod::Barycentric);

std::shared_ptr<TensorField3D> IVW_MODULE_TENSORVISBASE_API
subsample3D(std::shared_ptr<const TensorField3D> tensorField, size3_t newDimensions,
            const InterpolationMethod method = InterpolationMethod::Linear);

std::shared_ptr<TensorField3D> IVW_MODULE_TENSORVISBASE_API
subsample3D(std::shared_ptr<const TensorField3D> tensorField, size3_t newDimensions,
            const InterpolationMethod method, std::function<void(float)> fun);

std::shared_ptr<PosTexColorMesh> IVW_MODULE_TENSORVISBASE_API
generateBoundingBoxAdjacencyForTensorField(
    std::shared_ptr<const TensorField3D> tensorField, const vec4 color);

std::shared_ptr<BasicMesh> IVW_MODULE_TENSORVISBASE_API generateBoundingBoxForTensorField(
	std::shared_ptr<const TensorField3D> tensorField, const vec4 color);

std::shared_ptr<BasicMesh> IVW_MODULE_TENSORVISBASE_API generateSliceLevelGeometryForTensorField(
    std::shared_ptr<const TensorField3D> tensorField, const vec4 color,
    const CartesianCoordinateAxis axis, const size_t sliceNr);

template <typename C>
void forEachVoxel(const TensorField3D &v, C callback) {
    const auto &dims = v.getDimensions();
    size3_t pos;
    for (pos.z = 0; pos.z < dims.z; ++pos.z) {
        for (pos.y = 0; pos.y < dims.y; ++pos.y) {
            for (pos.x = 0; pos.x < dims.x; ++pos.x) {
                callback(pos);
            }
        }
    }
}

template <typename C>
void forEachVoxelParallel(const TensorField3D &v, C callback, size_t jobs = 0) {
    const auto &dims = v.getDimensions();

    if (jobs == 0) {
        const auto settings = InviwoApplication::getPtr()->getSettingsByType<SystemSettings>();
        jobs = 4 * settings->poolSize_.get();
        if (jobs == 0) {  // if poolsize is zero
            forEachVoxel(v, callback);
            return;
        }
    }

    std::vector<std::future<void>> futures;
    for (size_t job = 0; job < jobs; ++job) {
        const auto start = size3_t(0, 0, job * dims.z / jobs);
        const auto stop = size3_t(dims.x, dims.y, std::min(dims.z, (job + 1) * dims.z / jobs));

        futures.push_back(dispatchPool([&callback, start, stop]() {
            size3_t pos{0};

            for (pos.z = start.z; pos.z < stop.z; ++pos.z) {
                for (pos.y = start.y; pos.y < stop.y; ++pos.y) {
                    for (pos.x = start.x; pos.x < stop.x; ++pos.x) {
                        callback(pos);
                    }
                }
            }
        }));
    }

    for (const auto &e : futures) {
        e.wait();
    }
}

template <typename C>
void forEachFixel(const TensorField2D &v, C callback) {
    const auto &dims = v.getDimensions();
    size2_t pos;
    for (pos.y = 0; pos.y < dims.y; ++pos.y) {
        for (pos.x = 0; pos.x < dims.x; ++pos.x) {
            callback(pos);
        }
    }
}

template <typename C>
void forEachFixelParallel(const TensorField2D &v, C callback, size_t jobs = 0) {
    const auto &dims = v.getDimensions();

    if (jobs == 0) {
        const auto settings = InviwoApplication::getPtr()->getSettingsByType<SystemSettings>();
        jobs = 4 * settings->poolSize_.get();
        if (jobs == 0) {  // if poolsize is zero
            forEachFixel(v, callback);
            return;
        }
    }

    std::vector<std::future<void>> futures;
    for (size_t job = 0; job < jobs; ++job) {
        const auto start = size2_t(0, job * dims.y / jobs);
        const auto stop = size2_t(dims.x, std::min(dims.y, (job + 1) * dims.y / jobs));

        futures.push_back(dispatchPool([&callback, start, stop]() {
            size2_t pos{0};

            for (pos.y = start.y; pos.y < stop.y; ++pos.y) {
                for (pos.x = start.x; pos.x < stop.x; ++pos.x) {
                    callback(pos);
                }
            }

        }));
    }

    for (const auto &e : futures) {
        e.wait();
    }
}

}  // namespace tensorvisutil
}  // namespace inviwo
