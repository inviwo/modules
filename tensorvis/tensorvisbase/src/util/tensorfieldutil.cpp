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
 * 1. Redistributions of source code must retain the above copyright notice,
 *this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 *FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <modules/base/algorithm/meshutils.h>
#include <modules/opengl/texture/textureutils.h>
#include <inviwo/tensorvisbase/util/tensorfieldutil.h>
#include <inviwo/tensorvisbase/algorithm/tensorfieldsampling.h>

namespace inviwo {
namespace tensorutil {
void bindTensorFieldAsColorTexture(std::shared_ptr<Image>& texture,
                                   std::shared_ptr<const TensorField2D> tensorField, Shader& shader,
                                   TextureUnitContainer& textureUnits) {
    texture = tensorField->getImageRepresentation();

    utilgl::bindAndSetUniforms(shader, textureUnits, *texture, "tensorField", ImageType::ColorOnly);
}

void bindTensorFieldAsColorTexture(std::shared_ptr<Image>& texture, TensorField2DInport& inport,
                                   Shader& shader, TextureUnitContainer& textureUnits) {
    auto tensorField = inport.getData();

    texture = tensorField->getImageRepresentation();

    utilgl::bindAndSetUniforms(shader, textureUnits, *texture, "tensorField", ImageType::ColorOnly);
}

void bindTensorFieldAsColorTextures(std::shared_ptr<const TensorField3D>&, Shader*,
                                    TextureUnitContainer&) {
    // auto volumes = tensorField->getVolumeRepresentation();
}

std::shared_ptr<TensorField2D> subsample2D(std::shared_ptr<const TensorField2D> tensorField,
                                           size2_t newDimensions,
                                           const InterpolationMethod method) {
    std::vector<dmat2> dataNew;
    dataNew.resize(newDimensions.x * newDimensions.y);

    util::IndexMapper2D indexMapperNew(newDimensions);

    auto xFrac = static_cast<double>(tensorField->getDimensions().x - 1) /
                 static_cast<double>(newDimensions.x - 1);
    auto yFrac = static_cast<double>(tensorField->getDimensions().y - 1) /
                 static_cast<double>(newDimensions.y - 1);

    for (size_t x = 0; x < newDimensions.x; x++) {
        for (size_t y = 0; y < newDimensions.y; y++) {
            // Find position in old tensor field
            auto pos = dvec2(xFrac * static_cast<double>(x), yFrac * static_cast<double>(y));
            auto posNormalized =
                pos / dvec2(static_cast<double>(tensorField->getDimensions().x - 1),
                            static_cast<double>(tensorField->getDimensions().y - 1));

            // Sample old tensor field at position
            auto tensor = sample(tensorField, posNormalized, method);

            // Assign tensor at x,y
            dataNew[indexMapperNew(size2_t(x, y))] = tensor;
        }
    }

    return std::make_shared<TensorField2D>(newDimensions, dataNew);
}

std::shared_ptr<TensorField3D> IVW_MODULE_TENSORVISBASE_API
subsample3D(std::shared_ptr<const TensorField3D> tensorField, size3_t newDimensions,
            const InterpolationMethod method) {
    std::vector<dmat3> dataNew;
    dataNew.resize(newDimensions.x * newDimensions.y * newDimensions.z);

    util::IndexMapper3D indexMapperNew(newDimensions);

    auto xFrac = 1. / static_cast<double>(newDimensions.x - 1);
    auto yFrac = 1. / static_cast<double>(newDimensions.y - 1);
    auto zFrac = 1. / static_cast<double>(newDimensions.z - 1);

    for (size_t x = 0; x < newDimensions.x; x++) {
        for (size_t y = 0; y < newDimensions.y; y++) {
            for (size_t z = 0; z < newDimensions.z; z++) {
                // Find position in old tensor field
                auto pos = dvec3(xFrac * static_cast<double>(x), yFrac * static_cast<double>(y),
                                 zFrac * static_cast<double>(z));

                // Sample old tensor field at position
                auto tensor = sample(tensorField, pos, method);

                // Assign tensor at x,y
                dataNew[indexMapperNew(size3_t(x, y, z))] = tensor.second;
            }
        }
    }

    return std::make_shared<TensorField3D>(newDimensions, dataNew, tensorField->getExtent());
}

std::shared_ptr<TensorField3D> IVW_MODULE_TENSORVISBASE_API
subsample3D(std::shared_ptr<const TensorField3D> tensorField, size3_t newDimensions,
            const InterpolationMethod method, std::function<void(float)> fun) {
    std::vector<dmat3> dataNew;
    dataNew.resize(newDimensions.x * newDimensions.y * newDimensions.z);

    util::IndexMapper3D indexMapperNew(newDimensions);

    auto xFrac = 1. / static_cast<double>(newDimensions.x - 1);
    auto yFrac = 1. / static_cast<double>(newDimensions.y - 1);
    auto zFrac = 1. / static_cast<double>(newDimensions.z - 1);

    const auto numElements =
        static_cast<float>(newDimensions.x * newDimensions.y * newDimensions.z);
    float i = 0.f;

    for (size_t x = 0; x < newDimensions.x; x++) {
        for (size_t y = 0; y < newDimensions.y; y++) {
            for (size_t z = 0; z < newDimensions.z; z++) {
                fun(std::min(0.99f, i++ / numElements));
                // Find position in old tensor field
                auto pos = dvec3(xFrac * static_cast<double>(x), yFrac * static_cast<double>(y),
                                 zFrac * static_cast<double>(z));

                // Sample old tensor field at position
                auto tensor = sample(tensorField, pos, method);

                // Assign tensor at x,y
                dataNew[indexMapperNew(size3_t(x, y, z))] = tensor.second;
            }
        }
    }

    return std::make_shared<TensorField3D>(newDimensions, dataNew, tensorField->getExtent());
}

std::shared_ptr<PosTexColorMesh> generateBoundingBoxAdjacencyForTensorField(
    std::shared_ptr<const TensorField3D> tensorField, const vec4 color) {
    auto modelMatrix = tensorField->getBasisAndOffset();

    auto mesh1 = meshutil::boundingBoxAdjacency(modelMatrix, color);
    mesh1->setWorldMatrix(mat4(1));

    return mesh1;
}

std::shared_ptr<BasicMesh> generateBoundingBoxForTensorField(
    std::shared_ptr<const TensorField3D> tensorField, const vec4 color) {
    auto modelMatrix = tensorField->getBasisAndOffset();

    auto mesh1 = meshutil::cube(modelMatrix, color);
    mesh1->setWorldMatrix(mat4(1));

    return mesh1;
}

std::shared_ptr<BasicMesh> generateSliceLevelGeometryForTensorField(
    std::shared_ptr<const TensorField3D> tensorField, const vec4 color,
    const CartesianCoordinateAxis axis, const size_t sliceNr) {

    auto mesh = detail::generateSliceGeometry(tensorField, color, axis, sliceNr);

    auto indices = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::StripAdjacency);

    indices->add({3, 0, 1, 2, 3, 0, 1});

    return mesh;
}

std::shared_ptr<BasicMesh> generateSlicePlaneGeometryForTensorField(
    std::shared_ptr<const TensorField3D> tensorField, const vec4 color,
    const CartesianCoordinateAxis axis, const size_t sliceNr) {

    auto mesh = detail::generateSliceGeometry(tensorField, color, axis, sliceNr);

    auto indices = mesh->addIndexBuffer(DrawType::Triangles, ConnectivityType::None);

    indices->add(0);
    indices->add(1);
    indices->add(2);

    indices->add(2);
    indices->add(3);
    indices->add(0);

    return mesh;
}

namespace detail {
std::shared_ptr<BasicMesh> generateSliceGeometry(std::shared_ptr<const TensorField3D> tensorField,
                                                 const vec4 c, const CartesianCoordinateAxis axis,
                                                 const size_t sliceNr) {
    auto bounds = tensorField->getBounds<float>();
    auto bounds_t = tensorField->getBounds<size_t>();
    auto e = tensorField->getExtent<float>();
    auto offset = tensorField->getOffset();

    auto mesh = std::make_shared<BasicMesh>();

    switch (axis) {
        case CartesianCoordinateAxis::X: {
            auto xFrac = e.x / bounds.x;

            auto slice = float(std::min(sliceNr, bounds_t.x));

            xFrac *= slice;

            mesh->addVertex(offset + vec3(xFrac, 0.0f, 0.0f), vec3(1), vec3(0.0f), c);
            mesh->addVertex(offset + vec3(xFrac, e.y, 0.0f), vec3(1), vec3(0.0f, 1.0f, 0.0f), c);
            mesh->addVertex(offset + vec3(xFrac, e.y, e.z), vec3(1), vec3(1.0f), c);
            mesh->addVertex(offset + vec3(xFrac, 0.0f, e.z), vec3(1), vec3(1.0f, 0.0f, 0.0f), c);
        } break;
        case CartesianCoordinateAxis::Y: {
            auto yFrac = e.y / bounds.y;

            auto slice = float(std::min(sliceNr, bounds_t.y));

            yFrac *= slice;

            mesh->addVertex(offset + vec3(0.0f, yFrac, 0.0f), vec3(1), vec3(0.0f), c);
            mesh->addVertex(offset + vec3(e.x, yFrac, 0.0f), vec3(1), vec3(0.0f, 1.0f, 0.0f), c);
            mesh->addVertex(offset + vec3(e.x, yFrac, e.z), vec3(1), vec3(1.0f), c);
            mesh->addVertex(offset + vec3(0.0f, yFrac, e.z), vec3(1), vec3(1.0f, 0.0f, 0.0f), c);
        } break;
        case CartesianCoordinateAxis::Z: {
            auto zFrac = e.z / bounds.z;

            auto slice = float(std::min(sliceNr, bounds_t.z));

            zFrac *= slice;

            mesh->addVertex(offset + vec3(0.0f, 0.0f, zFrac), vec3(1), vec3(0.0f), c);
            mesh->addVertex(offset + vec3(e.x, 0.0f, zFrac), vec3(1), vec3(0.0f, 1.0f, 0.0f), c);
            mesh->addVertex(offset + vec3(e.x, e.y, zFrac), vec3(1), vec3(1.0f), c);
            mesh->addVertex(offset + vec3(0.0f, e.y, zFrac), vec3(1), vec3(1.0f, 0.0f, 0.0f), c);
        } break;
    }

    return mesh;
}
}  // namespace detail

}  // namespace tensorutil

}  // namespace inviwo
