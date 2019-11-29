#pragma once

#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/tensorvisbase/datastructures/tensorfield2d.h>
#include <inviwo/tensorvisbase/datastructures/tensorfield3d.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/datastructures/geometry/geometrytype.h>

namespace inviwo {
namespace detail {
std::shared_ptr<TensorField2D> IVW_MODULE_TENSORVISBASE_API
getSlice2D(std::shared_ptr<const TensorField3D> inTensorField, const CartesianCoordinateAxis axis,
           const size_t sliceNumber);

std::shared_ptr<TensorField3D> IVW_MODULE_TENSORVISBASE_API
getSlice3D(std::shared_ptr<const TensorField3D> inTensorField, const CartesianCoordinateAxis axis,
           const size_t sliceNumber);
}  // namespace detail

template <unsigned int N>
auto IVW_MODULE_TENSORVISBASE_API slice(std::shared_ptr<const TensorField3D> inTensorField,
                                        const CartesianCoordinateAxis axis,
                                        const size_t sliceNumber) {
    if constexpr (N == 2) {
        return detail::getSlice2D(inTensorField, axis, sliceNumber);
    } else {
        return detail::getSlice3D(inTensorField, axis, sliceNumber);
    }
}
}  // namespace inviwo
