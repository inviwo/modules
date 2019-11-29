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

    switch (axis) {
        case CartesianCoordinateAxis::X:
            for (size_t y = 0; y < dimensions.x; y++) {
                for (size_t z = 0; z < dimensions.y; z++) {
                    auto x = sliceNumber;
                    sliceData[indexMapper(size2_t(y, z))] =
                        tensorutil::getProjectedTensor(inTensorField->at(x, y, z).second, axis);
                }
            }
            break;
        case CartesianCoordinateAxis::Y:
            for (size_t x = 0; x < dimensions.x; x++) {
                for (size_t z = 0; z < dimensions.y; z++) {
                    auto y = sliceNumber;
                    sliceData[indexMapper(size2_t(x, z))] =
                        tensorutil::getProjectedTensor(inTensorField->at(x, y, z).second, axis);
                }
            }
            break;
        case CartesianCoordinateAxis::Z:
            for (size_t x = 0; x < dimensions.x; x++) {
                for (size_t y = 0; y < dimensions.y; y++) {
                    auto z = sliceNumber;
                    sliceData[indexMapper(size2_t(x, y))] =
                        tensorutil::getProjectedTensor(inTensorField->at(x, y, z).second, axis);
                }
            }
            break;
    }

    auto tensorField = std::make_shared<TensorField2D>(dimensions, sliceData);
    tensorField->setOffset(inTensorField->getOffset());

    return tensorField;
}

std::shared_ptr<TensorField3D> getSlice3D(std::shared_ptr<const TensorField3D> inTensorField,
                                          const CartesianCoordinateAxis axis,
                                          const size_t sliceNumber) {
    auto fieldDimensions = inTensorField->getDimensions();
    size3_t dimensions{0};
    double frac{0};

    auto stepSize = inTensorField->getSpacing();

    switch (axis) {
        case CartesianCoordinateAxis::X:
            dimensions = size3_t(1, fieldDimensions.y, fieldDimensions.z);
            frac = static_cast<double>(sliceNumber) * stepSize.x;
            break;
        case CartesianCoordinateAxis::Y:
            dimensions = size3_t(fieldDimensions.x, 1, fieldDimensions.z);
            frac = static_cast<double>(sliceNumber) * stepSize.y;
            break;
        case CartesianCoordinateAxis::Z:
            dimensions = size3_t(fieldDimensions.x, fieldDimensions.y, 1);
            frac = static_cast<double>(sliceNumber) * stepSize.z;
            break;
    }

    util::IndexMapper3D indexMapper(dimensions);

    std::vector<dmat3> sliceData{};
    sliceData.resize(dimensions.x * dimensions.y * dimensions.z);
    vec3 offset{0};

    switch (axis) {
        case CartesianCoordinateAxis::X:
            for (size_t y = 0; y < dimensions.y; y++) {
                for (size_t z = 0; z < dimensions.z; z++) {
                    auto x = sliceNumber;
                    sliceData[indexMapper(size3_t(0, y, z))] = inTensorField->at(x, y, z).second;
                    offset.x = frac;
                }
            }
            break;
        case CartesianCoordinateAxis::Y:
            for (size_t x = 0; x < dimensions.x; x++) {
                for (size_t z = 0; z < dimensions.z; z++) {
                    auto y = sliceNumber;
                    sliceData[indexMapper(size3_t(x, 0, z))] = inTensorField->at(x, y, z).second;
                    offset.y = frac;
                }
            }
            break;
        case CartesianCoordinateAxis::Z:
            for (size_t x = 0; x < dimensions.x; x++) {
                for (size_t y = 0; y < dimensions.y; y++) {
                    auto z = sliceNumber;
                    sliceData[indexMapper(size3_t(x, y, 0))] = inTensorField->at(x, y, z).second;
                    offset.z = frac;
                }
            }
            break;
    }

    auto tensorField =
        std::make_shared<TensorField3D>(dimensions, sliceData, inTensorField->getExtents(), frac);
    tensorField->setOffset(offset);

    return tensorField;
}
}  // namespace detail
}  // namespace inviwo
