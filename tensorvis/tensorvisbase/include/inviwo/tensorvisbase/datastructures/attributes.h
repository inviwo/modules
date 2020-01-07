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
using VectorBase = TypedAttributeBase<vec3>;

struct MajorEigenValue : ScalarBase {
    static constexpr inline std::string_view identifier{"Major Eigenvalue"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        std::vector<value_type> l1{};
        for (const auto& tensor : *tensors) {
            l1.emplace_back(util::eigenvalue<0>(tensor));
        }
        return l1;
    }
};

struct IntermediateEigenValue : ScalarBase {
    static constexpr inline std::string_view identifier{"Intermediate Eigenvalue"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N % 2) {
            std::vector<value_type> l2{};
            for (const auto& tensor : *tensors) {
                l2.emplace_back(util::eigenvalue<N / 2>(tensor));
            }
            return l2;
        }
    }
};

struct MinorEigenValue : ScalarBase {
    static constexpr inline std::string_view identifier{"Minor Eigenvalue"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        std::vector<value_type> ln{};
        for (const auto& tensor : *tensors) {
            ln.emplace_back(util::eigenvalue<N - 1>(tensor));
        }
        return ln;
    }
};

struct MajorEigenVector : VectorBase {
    static constexpr inline std::string_view identifier{"Major Eigenvector"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        std::vector<value_type> ev{};
        for (const auto& tensor : *tensors) {
            ev.emplace_back(util::eigenvector<0>(tensor));
        }
        return ev;
    }
};

struct IntermediateEigenVector : VectorBase {
    static constexpr inline std::string_view identifier{"Intermediate Eigenvector"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N % 2) {
            std::vector<value_type> ev{};
            for (const auto& tensor : *tensors) {
                ev.emplace_back(util::eigenvector<N / 2>(tensor));
            }
            return ev;
        }
    }
};

struct MinorEigenVector : VectorBase {
    static constexpr inline std::string_view identifier{"Minor Eigenvector"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        std::vector<value_type> ev{};
        for (const auto& tensor : *tensors) {
            ev.emplace_back(util::eigenvector<N - 1>(tensor));
        }
        return ev;
    }
};

struct I1 : ScalarBase {
    static constexpr inline std::string_view identifier{"I1"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        auto i1 = std::vector<value_type>();
        i1.reserve(tensors->size());
        for (const auto& tensor : *tensors) {
            i1.emplace_back(util::trace(tensor));
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
        if constexpr (N == 3) {
            auto i2 = std::vector<value_type>();
            for (const auto& tensor : *tensors) {
                i2.emplace_back(tensor[0][0] * tensor[1][1] + tensor[1][1] * tensor[2][2] +
                                tensor[0][0] * tensor[2][2] - tensor[0][1] * tensor[0][1] -
                                tensor[1][2] * tensor[1][2] - tensor[2][0] * tensor[2][0]);
            }
            return i2;
        }
    }
};

struct I3 : ScalarBase {
    static constexpr inline std::string_view identifier{"I3"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            auto i3 = std::vector<value_type>();
            for (const auto& tensor : *tensors) {
                i3.emplace_back(tensor[0][0] * tensor[1][1] * tensor[2][2] +
                                value_type(2) * tensor[0][1] * tensor[1][2] * tensor[2][0] -
                                tensor[0][1] * tensor[0][1] * tensor[2][2] -
                                tensor[1][2] * tensor[1][2] * tensor[0][0] -
                                tensor[2][0] * tensor[2][0] * tensor[1][1]);
            }
            return i3;
        }
    }
};

struct J1 : ScalarBase {
    static constexpr inline std::string_view identifier{"J1"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        auto j1 = std::vector<value_type>(tensors->size());
        std::fill(j1.begin(), j1.end(), value_type(0));
        return j1;
    }
};

struct J2 : ScalarBase {
    static constexpr inline std::string_view identifier{"J2"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            auto j2 = std::vector<value_type>();
            const auto i1 = I1::calculate(tensors, metaData);
            const auto i2 = I2::calculate(tensors, metaData);
            for (size_t i{0}; i < tensors->size(); ++i) {
                j2.emplace_back((value_type(1. / 3.)) * i1[i] * i1[i] - i2[i]);
                i++;
            }
            return j2;
        }
    }
};

struct J3 : ScalarBase {
    static constexpr inline std::string_view identifier{"J3"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            auto j3 = std::vector<value_type>();
            const auto i1 = I1::calculate(tensors, metaData);
            const auto i2 = I2::calculate(tensors, metaData);
            const auto i3 = I3::calculate(tensors, metaData);

            for (size_t i{0}; i < tensors->size(); ++i) {
                j3.emplace_back(value_type(2. / 27.) * i1[i] * i1[i] * i1[i] -
                                value_type(1. / 3.) * i1[i] * i2[i] + i3[i]);
            }
            return j3;
        }
    }
};

struct LodeAngle : ScalarBase {
    static constexpr inline std::string_view identifier{"Lode angle"};

    template <unsigned int N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            auto lodeAngle = std::vector<value_type>();
            constexpr auto a =
                (value_type(3.) * util::constexpr_sqrt(value_type(3.))) * value_type(.5);
            constexpr auto third = value_type(1. / 3.);

            const auto j2 = J2::calculate(tensors, metaData);
            const auto j3 = J3::calculate(tensors, metaData);

            for (size_t i{0}; i < tensors->size(); ++i) {
                const auto b = j3[i] / std::pow(j2[i], value_type(1.5));

                lodeAngle.emplace_back(third * std::acos(a * b));
            }
            return lodeAngle;
        }
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
    std::tuple<
        MajorEigenValue,
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
        FrobeniusNorm
    >;
// clang-format on

}  // namespace attributes
}  // namespace inviwo