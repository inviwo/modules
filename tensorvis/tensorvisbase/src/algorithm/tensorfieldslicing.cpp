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

#include <inviwo/tensorvisbase/algorithm/tensorfieldslicing.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>

namespace inviwo {
namespace detail {
std::shared_ptr<TensorField2D> getSlice2D(std::shared_ptr<const TensorField3D> inTensorField,
                                          const CartesianCoordinateAxis axis,
                                          const size_t sliceNumber) {
    auto fieldDimensions = inTensorField->getDimensions();
    size2_t dimensions{0};

    switch (axis) {
        case CartesianCoordinateAxis::X:
            dimensions = size2_t(fieldDimensions.y, fieldDimensions.z);
            break;
        case CartesianCoordinateAxis::Y:
            dimensions = size2_t(fieldDimensions.x, fieldDimensions.z);
            break;
        case CartesianCoordinateAxis::Z:
            dimensions = size2_t(fieldDimensions.x, fieldDimensions.y);
            break;
    }

    util::IndexMapper2D indexMapper(dimensions);

    std::vector<dmat2> sliceData;
    sliceData.resize(dimensions.x * dimensions.y);

    const vec3 sourceExtent{inTensorField->getExtent()};
    const vec3 sourceOffset{inTensorField->getOffset()};

    vec2 extent{1.0f};
    vec2 offset{0.0f};

    switch (axis) {
        case CartesianCoordinateAxis::X:
            for (size_t z = 0; z < dimensions.y; z++) {
                for (size_t y = 0; y < dimensions.x; y++) {
                    auto x = sliceNumber;
                    sliceData[indexMapper(size2_t(y, z))] =
                        tensorutil::getProjectedTensor(inTensorField->at(x, y, z).second, axis);
                }
            }
            extent = vec2{sourceExtent.y, sourceExtent.z};
            offset = vec2{sourceOffset.y, sourceOffset.z};
            break;
        case CartesianCoordinateAxis::Y:
            for (size_t z = 0; z < dimensions.y; z++) {
                for (size_t x = 0; x < dimensions.x; x++) {
                    auto y = sliceNumber;
                    sliceData[indexMapper(size2_t(x, z))] =
                        tensorutil::getProjectedTensor(inTensorField->at(x, y, z).second, axis);
                }
            }
            extent = vec2{sourceExtent.x, sourceExtent.z};
            offset = vec2{sourceOffset.x, sourceOffset.z};
            break;
        case CartesianCoordinateAxis::Z:
            for (size_t y = 0; y < dimensions.y; y++) {
                for (size_t x = 0; x < dimensions.x; x++) {
                    auto z = sliceNumber;
                    sliceData[indexMapper(size2_t(x, y))] =
                        tensorutil::getProjectedTensor(inTensorField->at(x, y, z).second, axis);
                }
            }
            extent = vec2{sourceExtent.x, sourceExtent.y};
            offset = vec2{sourceOffset.x, sourceOffset.y};
            break;
    }

    auto tensorField = std::make_shared<TensorField2D>(dimensions, sliceData, extent);
    tensorField->setOffset(offset);

    return tensorField;
}

std::shared_ptr<TensorField3D> getSlice3D(std::shared_ptr<const TensorField3D> inTensorField,
                                          const CartesianCoordinateAxis axis,
                                          const size_t sliceNumber) {
    auto fieldDimensions = inTensorField->getDimensions();
    size3_t dimensions{0};
    float frac{0.0f};

    auto stepSize = inTensorField->getSpacing<float>();

    switch (axis) {
        case CartesianCoordinateAxis::X:
            dimensions = size3_t(1, fieldDimensions.y, fieldDimensions.z);
            frac = static_cast<float>(sliceNumber) * stepSize.x;
            break;
        case CartesianCoordinateAxis::Y:
            dimensions = size3_t(fieldDimensions.x, 1, fieldDimensions.z);
            frac = static_cast<float>(sliceNumber) * stepSize.y;
            break;
        case CartesianCoordinateAxis::Z:
            dimensions = size3_t(fieldDimensions.x, fieldDimensions.y, 1);
            frac = static_cast<float>(sliceNumber) * stepSize.z;
            break;
    }

    util::IndexMapper3D indexMapper(dimensions);

    std::vector<dmat3> sliceData{};
    sliceData.resize(dimensions.x * dimensions.y * dimensions.z);
    mat3 basis{inTensorField->getBasis()};
    vec3 offset{inTensorField->getOffset()};

    switch (axis) {
        case CartesianCoordinateAxis::X:
            for (size_t z = 0; z < dimensions.z; z++) {
                for (size_t y = 0; y < dimensions.y; y++) {
                    auto x = sliceNumber;
                    sliceData[indexMapper(size3_t(0, y, z))] = inTensorField->at(x, y, z).second;
                }
            }
            basis[0] = vec3{0.0f};
            offset.x += frac;
            break;
        case CartesianCoordinateAxis::Y:
            for (size_t z = 0; z < dimensions.z; z++) {
                for (size_t x = 0; x < dimensions.x; x++) {
                    auto y = sliceNumber;
                    sliceData[indexMapper(size3_t(x, 0, z))] = inTensorField->at(x, y, z).second;
                }
            }
            basis[1] = vec3{0.0f};
            offset.y += frac;
            break;
        case CartesianCoordinateAxis::Z:
            for (size_t y = 0; y < dimensions.y; y++) {
                for (size_t x = 0; x < dimensions.x; x++) {
                    auto z = sliceNumber;
                    sliceData[indexMapper(size3_t(x, y, 0))] = inTensorField->at(x, y, z).second;
                }
            }
            basis[2] = vec3{0.0f};
            offset.z += frac;
            break;
    }

    auto tensorField =
        std::make_shared<TensorField3D>(dimensions, sliceData, inTensorField->getExtent(), frac);

    tensorField->setBasis(basis);
    tensorField->setOffset(offset);

    return tensorField;
}
}  // namespace detail
}  // namespace inviwo
