/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2023-2024 Inviwo Foundation
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

#include <inviwo/ttk/ttkmoduledefine.h>

#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/util/exception.h>
#include <inviwo/core/util/glm.h>

#include <vtkAbstractArray.h>
#include <vtkArrayDispatch.h>

namespace inviwo {

namespace vtk {

template <size_t N, typename TypedArray>
auto getTuple(TypedArray* array, vtkIdType id) {
    using SrcT = decltype(array->GetTypedComponent(0, 0));

    // Avoid long it if is the same size as int
    using Tmp1T = std::conditional_t<std::is_same_v<SrcT, unsigned long> &&
                                         sizeof(unsigned long) == sizeof(unsigned int),
                                     unsigned int, SrcT>;
    using Tmp2T =
        std::conditional_t<std::is_same_v<Tmp1T, long> && sizeof(long) == sizeof(int), int, Tmp1T>;

    using DstT = Tmp2T;

    if constexpr (N == 1) {
        return DstT{array->GetTypedComponent(id, 0)};
    } else if constexpr (N == 2) {
        return glm::vec<2, DstT>{array->GetTypedComponent(id, 0), array->GetTypedComponent(id, 1)};
    } else if constexpr (N == 3) {
        return glm::vec<3, DstT>{array->GetTypedComponent(id, 0), array->GetTypedComponent(id, 1),
                                 array->GetTypedComponent(id, 2)};
    } else if constexpr (N == 4) {
        return glm::vec<4, DstT>{array->GetTypedComponent(id, 0), array->GetTypedComponent(id, 1),
                                 array->GetTypedComponent(id, 2), array->GetTypedComponent(id, 3)};
    } else {
        throw Exception("no matching type");
    }
}

template <size_t N, typename TypedArray, typename Transform, typename Repeat>
auto getVectorOfTuple(TypedArray* array, Transform transform, Repeat repeat) {
    using DType = decltype(std::invoke(transform, getTuple<N>(array, 0)));
    const auto nTuples = array->GetNumberOfTuples();
    std::vector<DType> data;
    data.reserve(nTuples);
    for (int i = 0; i < nTuples; ++i) {
        auto val = std::invoke(transform, getTuple<N>(array, i));
        for (auto j = repeat(i); j != 0; --j) {
            data.push_back(val);
        }
    }
    return data;
}

template <typename Transform, typename Repeat>
std::shared_ptr<BufferBase> arrayToBuffer(vtkDataArray* array, Transform transform, Repeat repeat) {
    std::shared_ptr<BufferBase> buffer;

    auto worker = [&](auto* typedArray) {
        const auto nComp = typedArray->GetNumberOfComponents();
        if (nComp == 1) {
            buffer = util::makeBuffer(getVectorOfTuple<1>(typedArray, transform, repeat));
        } else if (nComp == 2) {
            buffer = util::makeBuffer(getVectorOfTuple<2>(typedArray, transform, repeat));
        } else if (nComp == 3) {
            buffer = util::makeBuffer(getVectorOfTuple<3>(typedArray, transform, repeat));
        } else if (nComp == 4) {
            buffer = util::makeBuffer(getVectorOfTuple<4>(typedArray, transform, repeat));
        } else {
            throw Exception("no matching type");
        }
    };
    if (!vtkArrayDispatch::Dispatch::Execute(array, worker)) {
        throw Exception("no matching type");
    }

    return buffer;
}

template <size_t N, typename TypedArray, typename Transform>
auto getVolumeRAMOfTuple(size3_t dimensions, TypedArray* array, Transform transform) {
    using DType = decltype(std::invoke(transform, getTuple<N>(array, 0)));
    const auto nTuples = array->GetNumberOfTuples();

    auto ram = std::make_shared<VolumeRAMPrecision<DType>>(dimensions);

    if (static_cast<vtkIdType>(glm::compMul(dimensions)) != nTuples) {
        throw Exception("Invalid dims");
    }

    auto data = ram->getDataTyped();

    for (int i = 0; i < nTuples; ++i) {
        const auto val = std::invoke(transform, getTuple<N>(array, i));
        data[i] = val;
    }
    return ram;
}

template <typename Transform>
std::shared_ptr<VolumeRAM> arrayToVolumeRAM(size3_t dimensions, vtkDataArray* array,
                                            Transform transform) {
    std::shared_ptr<VolumeRAM> ram;

    auto worker = [&](auto* typedArray) {
        const auto nComp = typedArray->GetNumberOfComponents();
        if (nComp == 1) {
            ram = getVolumeRAMOfTuple<1>(dimensions, typedArray, transform);
        } else if (nComp == 2) {
            ram = getVolumeRAMOfTuple<2>(dimensions, typedArray, transform);
        } else if (nComp == 3) {
            ram = getVolumeRAMOfTuple<3>(dimensions, typedArray, transform);
        } else if (nComp == 4) {
            ram = getVolumeRAMOfTuple<4>(dimensions, typedArray, transform);
        } else {
            throw Exception("no matching type");
        }
    };
    if (!vtkArrayDispatch::Dispatch::Execute(array, worker)) {
        throw Exception("no matching type");
    }

    return ram;
}

}  // namespace vtk

}  // namespace inviwo
