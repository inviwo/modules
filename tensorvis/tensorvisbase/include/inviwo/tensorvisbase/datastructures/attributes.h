#pragma once

#include <string_view>
#include <inviwo/dataframe/datastructures/dataframe.h>

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
using VectorBase = TypedAttributeBase<vec3>;

struct MajorEigenValue : ScalarBase {
    static constexpr inline std::string_view identifier{"Major Eigenvalue"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct IntermediateEigenValue : ScalarBase {
    static constexpr inline std::string_view identifier{"Intermediate Eigenvalue"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct MinorEigenValue : ScalarBase {
    static constexpr inline std::string_view identifier{"Minor Eigenvalue"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct MinorEigenVector : VectorBase {
    static constexpr inline std::string_view identifier{"Minor Eigenvector"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct IntermediateEigenVector : VectorBase {
    static constexpr inline std::string_view identifier{"Intermediate Eigenvector"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct MajorEigenVector : VectorBase {
    static constexpr inline std::string_view identifier{"Major Eigenvector"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct I1 : ScalarBase {
    static constexpr inline std::string_view identifier{"I1"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        auto i1 = std::vector<value_type>();
        value_type sum{ 0 };
        for (const auto& tensor : *tensors) {
            for (unsigned int i{ 0 }; i < N; ++i) {
                sum += tensor[N][N];
            }
            i1.emplace_back(sum);
        }
        return i1;
    }
};

struct I2 : ScalarBase {
    static constexpr inline std::string_view identifier{"I2"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct I3 : ScalarBase {
    static constexpr inline std::string_view identifier{"I3"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct J1 : ScalarBase {
    static constexpr inline std::string_view identifier{"J1"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct J2 : ScalarBase {
    static constexpr inline std::string_view identifier{"J2"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct J3 : ScalarBase {
    static constexpr inline std::string_view identifier{"J3"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct LodeAngle : ScalarBase {
    static constexpr inline std::string_view identifier{"Lode angle"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct Anisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Anisotropy"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct LinearAnisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Linear anisotropy"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct PlanarAnisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Planar anisotropy"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct SphericalAnisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Spherical anisotropy"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

struct FrobeniusNorm : ScalarBase {
    static constexpr inline std::string_view identifier{"Frobenius Norm"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
};

// clang-format off
            using types =
                std::tuple<MajorEigenValue,
                IntermediateEigenValue,
                MinorEigenValue,
                MajorEigenVector,
                IntermediateEigenVector,
                MinorEigenVector,
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
                FrobeniusNorm>;
// clang-format on

}  // namespace attributes
}  // namespace inviwo