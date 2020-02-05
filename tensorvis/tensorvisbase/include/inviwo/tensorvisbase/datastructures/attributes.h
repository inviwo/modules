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
        std::shared_ptr<const DataFrame> metaData) {
        std::vector<typename VectorBase<N>::value_type> ev{};
        for (const auto& tensor : *tensors) {
            ev.emplace_back(util::eigenvector<0>(tensor));
        }
        return ev;
    }
};

template <glm::length_t N>
struct IntermediateEigenVector : VectorBase<N> {
    static constexpr inline std::string_view identifier{"Intermediate Eigenvector"};

    static std::vector<typename VectorBase<N>::value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, typename VectorBase<N>::scalar_type>>>
            tensors,
        std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N % 2) {
            std::vector<typename VectorBase<N>::value_type> ev{};
            for (const auto& tensor : *tensors) {
                ev.emplace_back(util::eigenvector<N / 2>(tensor));
            }
            return ev;
        }
    }
};

template <glm::length_t N>
struct MinorEigenVector : VectorBase<N> {
    static constexpr inline std::string_view identifier{"Minor Eigenvector"};

    static std::vector<typename VectorBase<N>::value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, typename VectorBase<N>::scalar_type>>>
            tensors,
        std::shared_ptr<const DataFrame> metaData) {
        std::vector<typename VectorBase<N>::value_type> ev{};
        for (const auto& tensor : *tensors) {
            ev.emplace_back(util::eigenvector<N - 1>(tensor));
        }
        return ev;
    }
};

struct I1 : ScalarBase {
    static constexpr inline std::string_view identifier{"I1"};

    template <glm::length_t N>
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

    template <glm::length_t N>
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

    template <glm::length_t N>
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

    template <glm::length_t N>
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

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            auto j2 = std::vector<value_type>();
            const auto i1 = I1::calculate(tensors, metaData);
            const auto i2 = I2::calculate(tensors, metaData);
            for (size_t i{0}; i < tensors->size(); ++i) {
                j2.emplace_back((value_type(1. / 3.)) * i1[i] * i1[i] - i2[i]);
            }
            return j2;
        }
    }
};

struct J3 : ScalarBase {
    static constexpr inline std::string_view identifier{"J3"};

    template <glm::length_t N>
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

    template <glm::length_t N>
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

namespace {
template <glm::length_t N, typename T>
std::vector<std::array<T, N>> sortedEigenValues(
    std::shared_ptr<const std::vector<glm::mat<N, N, T>>> tensors,
    std::shared_ptr<const DataFrame> metaData) {

    std::vector<T> mae;
    std::vector<T> ine;
    std::vector<T> mie;

    if (!metaData->getColumn(std::string(MajorEigenValue::identifier))) {
        mae = MajorEigenValue::calculate(tensors, metaData);
    }
    if (!metaData->getColumn(std::string(IntermediateEigenValue::identifier))) {
        ine = IntermediateEigenValue::calculate(tensors, metaData);
    }
    if (!metaData->getColumn(std::string(MinorEigenValue::identifier))) {
        mie = MinorEigenValue::calculate(tensors, metaData);
    }

    const auto& majorEigenValues =
        mae.empty() ? std::dynamic_pointer_cast<const TemplateColumn<T>>(
                          metaData->getColumn(std::string(MajorEigenValue::identifier)))
                          ->getTypedBuffer()
                          ->getRAMRepresentation()
                          ->getDataContainer()
                    : mae;
    const auto& intermediateEigenValues =
        ine.empty() ? std::dynamic_pointer_cast<const TemplateColumn<T>>(
                          metaData->getColumn(std::string(IntermediateEigenValue::identifier)))
                          ->getTypedBuffer()
                          ->getRAMRepresentation()
                          ->getDataContainer()
                    : ine;
    const auto& minorEigenValues =
        mie.empty() ? std::dynamic_pointer_cast<const TemplateColumn<T>>(
                          metaData->getColumn(std::string(MinorEigenValue::identifier)))
                          ->getTypedBuffer()
                          ->getRAMRepresentation()
                          ->getDataContainer()
                    : mie;

    auto ev = std::vector<std::array<T, N>>{};

    for (size_t i = 0; i < majorEigenValues.size(); i++) {
        std::array<T, N> eigenValues{majorEigenValues[i], intermediateEigenValues[i],
                                     minorEigenValues[i]};

        std::transform(eigenValues.begin(), eigenValues.end(), eigenValues.begin(),
                       [](const T& val) { return glm::abs(val); });

        std::sort(eigenValues.begin(), eigenValues.end(),
                  [](const T& valA, const T& valB) { return valA > valB; });

        ev.push_back(eigenValues);
    }

    return ev;
}
}  // namespace

struct Anisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Anisotropy"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            std::vector<scalar_type> anisotropy{};
            auto ev = sortedEigenValues(tensors, metaData);
            for (const auto& eigenValues : ev) {
                auto denominator = eigenValues[0] + eigenValues[N / 2] + eigenValues[N - 1];
                if (denominator < std::numeric_limits<scalar_type>::epsilon())
                    denominator = std::numeric_limits<scalar_type>::epsilon();

                anisotropy.emplace_back(std::abs(eigenValues[0] - eigenValues[2]));
            }
            return anisotropy;
        }
    }
};

struct LinearAnisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Linear anisotropy"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            std::vector<scalar_type> linearAnisotropy{};
            auto ev = sortedEigenValues(tensors, metaData);
            for (const auto& eigenValues : ev) {
                auto denominator = eigenValues[0] + eigenValues[N / 2] + eigenValues[N - 1];
                if (denominator < std::numeric_limits<scalar_type>::epsilon())
                    denominator = std::numeric_limits<scalar_type>::epsilon();

                linearAnisotropy.emplace_back((eigenValues[0] - eigenValues[1]) / denominator);
            }
            return linearAnisotropy;
        }
    }
};

struct PlanarAnisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Planar anisotropy"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            std::vector<scalar_type> planarAnisotropy{};
            auto ev = sortedEigenValues(tensors, metaData);
            for (const auto& eigenValues : ev) {
                auto denominator = eigenValues[0] + eigenValues[N / 2] + eigenValues[N - 1];
                if (denominator < std::numeric_limits<scalar_type>::epsilon())
                    denominator = std::numeric_limits<scalar_type>::epsilon();

                planarAnisotropy.emplace_back((scalar_type(2) * (eigenValues[1] - eigenValues[2])) /
                                              denominator);
            }
            return planarAnisotropy;
        }
    }
};

struct SphericalAnisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Spherical anisotropy"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            std::vector<scalar_type> sphericalAnisotropy{};
            auto ev = sortedEigenValues(tensors, metaData);
            for (const auto& eigenValues : ev) {
                auto denominator = eigenValues[0] + eigenValues[N / 2] + eigenValues[N - 1];
                if (denominator < std::numeric_limits<scalar_type>::epsilon())
                    denominator = std::numeric_limits<scalar_type>::epsilon();

                sphericalAnisotropy.emplace_back((scalar_type(3) * eigenValues[2]) / denominator);
            }
            return sphericalAnisotropy;
        }
    }
};

struct FrobeniusNorm : ScalarBase {
    static constexpr inline std::string_view identifier{"Frobenius Norm"};

    template <glm::length_t N>
    static std::vector<value_type> calculate(
        std::shared_ptr<const std::vector<glm::mat<N, N, scalar_type>>> tensors,
        std::shared_ptr<const DataFrame> metaData) {
        return std::vector<value_type>();
    }
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
        Trace,
        Norm,
        MajorEigenValue,
        IntermediateEigenValue,
        MinorEigenValue,
        MajorEigenVector3D,
        IntermediateEigenVector3D,
        MinorEigenVector3D,
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
        Trace,
        Norm,
        MajorEigenValue,
        MinorEigenValue,
        MajorEigenVector2D,
        MinorEigenVector2D
        //Anisotropy
    > ;
// clang-format on

}  // namespace attributes
}  // namespace inviwo

#include "attributes.inl"
