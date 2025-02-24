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

#include <ivwtensorvis/pytensorvis.h>

#include <inviwo/core/util/glmvec.h>
#include <inviwo/core/util/glmmat.h>
#include <inviwo/core/util/assertion.h>
#include <inviwo/core/util/exception.h>
#include <inviwo/core/util/formatdispatching.h>
#include <inviwo/core/util/zip.h>
#include <modules/python3/pyportutils.h>
#include <modules/python3/pybindutils.h>
#include <inviwo/tensorvisbase/datastructures/tensorfield3d.h>
#include <inviwo/tensorvisbase/ports/tensorfieldport.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>

#include <fmt/format.h>

namespace py = pybind11;

namespace inviwo {

void exposeTensorVis(pybind11::module& m) {

    py::classh<TensorField3D>(m, "TensorField3D")
        .def(py::init([](size3_t dims, std::vector<dmat3> data, vec3 extent,
                         float sliceCoord) -> TensorField3D {
            return TensorField3D{dims, data, extent, sliceCoord};
        }))
        .def(
            py::init([](py::array arr, vec3 extent, float sliceCoord) -> TensorField3D {
                auto ndim = arr.ndim();
                ivwAssert(ndim == 4 || ndim == 5, "ndims must be either 4 or 5");

                if (ndim == 4 && arr.shape(3) != 9) {
                    throw Exception(IVW_CONTEXT_CUSTOM("pytensorvis.TensorField3D"),
                                    "Invalid shape: expected 9 components in last dimension of "
                                    "numpy array ([{}, {}, {}, {}])",
                                    arr.shape(0), arr.shape(1), arr.shape(2), arr.shape(3));
                } else if (ndim == 5 && (arr.shape(3) != 3 || arr.shape(4) != 3)) {
                    throw Exception(
                        IVW_CONTEXT_CUSTOM("pytensorvis.TensorField3D"),
                        "Invalid shape: expected 3 components in last two dimensions of "
                        "numpy array ([{}, {}, {}, {}, {}])",
                        arr.shape(0), arr.shape(1), arr.shape(2), arr.shape(3), arr.shape(4));
                }

                auto df = pyutil::getDataFormat(1, arr);

                if (pybind11::array::c_style == (arr.flags() & pybind11::array::c_style)) {
                    return dispatching::singleDispatch<TensorField3D, dispatching::filter::Scalars>(
                        df->getId(), [&]<typename Type>() {
                            size3_t dims{arr.shape(2), arr.shape(1), arr.shape(0)};
                            std::vector<dmat3> tensors(glm::compMul(dims));

                            for (auto&& [idx, tensor] : util::enumerate(tensors)) {
                                if constexpr (std::is_same_v<Type, double>) {
                                    tensor = glm::make_mat3<double>(
                                        static_cast<const double*>(arr.data(0)) + idx * 9);
                                } else {
                                    tensor = dmat3{glm::make_mat3<Type>(
                                        static_cast<const Type*>(arr.data(0)) + idx * 9)};
                                }
                            }

                            return TensorField3D{dims, tensors, extent, sliceCoord};
                        });
                } else {
                    throw Exception(
                        "Unable to create a Buffer from array: The array is not in contiguous C "
                        "order. Use numpy.ascontiguousarray() or similar to ensure this.",
                        IVW_CONTEXT_CUSTOM("pybindutils"));
                }
            }),
            py::arg("arr"), py::arg("extent") = vec3{1.0f}, py::arg("sliceCoord") = 0.0f)
        .def_property_readonly("dimensions", &TensorField3D::getDimensions)
        .def_property("modelMatrix", &TensorField3D::getModelMatrix, &TensorField3D::setModelMatrix)
        .def_property("worldMatrix", &TensorField3D::getWorldMatrix, &TensorField3D::setWorldMatrix)
        .def_property("basis", &TensorField3D::getBasis, &TensorField3D::setBasis)
        .def_property("offset", &TensorField3D::getOffset, &TensorField3D::setOffset)
        .def_property_readonly("data", [&](TensorField3D* tensorfield) -> py::array {
            const size3_t dims{tensorfield->getDimensions()};
            const size_t matSizeInBytes = 9 * sizeof(double);

            std::vector<size_t> shape = {dims.z, dims.y, dims.x, 9};
            std::vector<size_t> strides = {matSizeInBytes * dims.y * dims.x,
                                           matSizeInBytes * dims.x, matSizeInBytes, sizeof(double)};

            auto data = tensorfield->tensors();
            return py::array(pyutil::toNumPyFormat(DataFormatBase::get(DataFormatId::Float64)),
                             shape, strides, data.data(), py::cast<>(1));
        });

    exposeStandardDataPorts<TensorField3D>(m, "TensorField3D");
}

}  // namespace inviwo
