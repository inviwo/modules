/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2020 Inviwo Foundation
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

#include <string_view>
#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>

namespace inviwo {

namespace attributes {
/**
 * This base class exists so we can check for example std::is_base_of<AttributeBase, I1>
 */
struct AttributeBase {
    virtual void foo() = 0;
};

template <typename T>
struct TypedAttributeBase : AttributeBase {
    using value_type = T;
    using scalar_type = typename util::value_type<T>::type;
};

using ScalarBase = TypedAttributeBase<float>;

template <glm::length_t N, typename T = float>
struct VectorBase : TypedAttributeBase<glm::vec<N, T>> {
    static constexpr glm::length_t extent = N;
};

struct Trace : ScalarBase {
    static constexpr inline std::string_view identifier{"Trace"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct Norm : ScalarBase {
    static constexpr inline std::string_view identifier{"Norm"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct MajorEigenValue : ScalarBase {
    static constexpr inline std::string_view identifier{"Major Eigenvalue"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct IntermediateEigenValue : ScalarBase {
    static constexpr inline std::string_view identifier{"Intermediate Eigenvalue"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct MinorEigenValue : ScalarBase {
    static constexpr inline std::string_view identifier{"Minor Eigenvalue"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

template <glm::length_t N>
struct MajorEigenVector : VectorBase<N> {
    static constexpr inline std::string_view identifier{"Major Eigenvector"};

    static std::vector<typename VectorBase<N>::value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, typename VectorBase<N>::scalar_type>>>
            tensors,
        std::shared_ptr<const DataFrame> metaData);
};

template <glm::length_t N>
struct IntermediateEigenVector : VectorBase<N> {
    static constexpr inline std::string_view identifier{"Intermediate Eigenvector"};

    static std::vector<typename VectorBase<N>::value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, typename VectorBase<N>::scalar_type>>>
            tensors,
        std::shared_ptr<const DataFrame> metaData);
};

template <glm::length_t N>
struct MinorEigenVector : VectorBase<N> {
    static constexpr inline std::string_view identifier{"Minor Eigenvector"};

    static std::vector<typename VectorBase<N>::value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, typename VectorBase<N>::scalar_type>>>
            tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct I1 : ScalarBase {
    static constexpr inline std::string_view identifier{"I1"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct I2 : ScalarBase {
    static constexpr inline std::string_view identifier{"I2"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct I3 : ScalarBase {
    static constexpr inline std::string_view identifier{"I3"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct J1 : ScalarBase {
    static constexpr inline std::string_view identifier{"J1"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct J2 : ScalarBase {
    static constexpr inline std::string_view identifier{"J2"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct J3 : ScalarBase {
    static constexpr inline std::string_view identifier{"J3"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct LodeAngle : ScalarBase {
    static constexpr inline std::string_view identifier{"Lode angle"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct Anisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Anisotropy"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct LinearAnisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Linear anisotropy"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct PlanarAnisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Planar anisotropy"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct SphericalAnisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Spherical anisotropy"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

struct FrobeniusNorm : ScalarBase {
    static constexpr inline std::string_view identifier{"Frobenius Norm"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData);
};

// Aliases for different domain languages
using MajorPrincipalStress = MajorEigenValue;
using IntermediatePrincipalStress = IntermediateEigenValue;
using MinorPrincipalStress = MinorEigenValue;

using Lambda1 = MajorEigenValue;
using Lambda2 = IntermediateEigenValue;
using Lambda3 = MinorEigenValue;

using MajorEigenVector2D = MajorEigenVector<2>;
using IntermediateEigenVector2D = IntermediateEigenVector<2>;
using MinorEigenVector2D = MinorEigenVector<2>;

using MajorEigenVector3D = MajorEigenVector<3>;
using IntermediateEigenVector3D = IntermediateEigenVector<3>;
using MinorEigenVector3D = MinorEigenVector<3>;

// clang-format off
using types3D =
    std::tuple<
        MajorEigenValue,
        IntermediateEigenValue,
        MinorEigenValue,
        MajorEigenVector3D,
        IntermediateEigenVector3D,
        MinorEigenVector3D,
        Trace,
        Norm,
        I1,
        I2,
        I3,
        J1,
        J2,
        J3,
        LodeAngle,
        Anisotropy,
        LinearAnisotropy,
        PlanarAnisotropy,
        SphericalAnisotropy,
        FrobeniusNorm
    >;

using types2D =
    std::tuple <
        MajorEigenValue,
        MinorEigenValue,
        MajorEigenVector2D,
        MinorEigenVector2D,
        Trace,
        Norm
        //Anisotropy
    > ;
// clang-format on

}  // namespace attributes
}  // namespace inviwo

#include "attributes.inl"
