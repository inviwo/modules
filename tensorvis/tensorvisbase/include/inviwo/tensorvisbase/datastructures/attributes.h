#pragma once

#include <string_view>

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
};

using ScalarBase = TypedAttributeBase<float>;
using VectorBase = TypedAttributeBase<vec3>;

struct MajorEigenValue : ScalarBase {
    static constexpr inline std::string_view identifier{"MajorEigenValue"};
};

struct IntermediateEigenValue : ScalarBase {
    static constexpr inline std::string_view identifier{"IntermediateEigenValue"};
};

struct MinorEigenValue : ScalarBase {
    static constexpr inline std::string_view identifier{"MinorEigenValue"};
};

struct I1 : ScalarBase {
    static constexpr inline std::string_view identifier{"I1"};
};

struct I2 : ScalarBase {
    static constexpr inline std::string_view identifier{"I2"};
};

struct I3 : ScalarBase {
    static constexpr inline std::string_view identifier{"I3"};
};

struct J1 : ScalarBase {
    static constexpr inline std::string_view identifier{"J1"};
};

struct J2 : ScalarBase {
    static constexpr inline std::string_view identifier{"J2"};
};

struct J3 : ScalarBase {
    static constexpr inline std::string_view identifier{"J3"};
};

struct LodeAngle : ScalarBase {
    static constexpr inline std::string_view identifier{"Lode angle"};
};

struct Anisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Anisotropy"};
};

struct LinearAnisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Linear anisotropy"};
};

struct PlanarAnisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Planar anisotropy"};
};

struct SphericalAnisotropy : ScalarBase {
    static constexpr inline std::string_view identifier{"Spherical anisotropy"};
};

struct FrobeniusNorm : ScalarBase {
    static constexpr inline std::string_view identifier{"Frobenius Norm"};
};

struct MinorEigenVector : VectorBase {
    static constexpr inline std::string_view identifier{"Minor Eigenvector"};
};

struct IntermediateEigenVector : VectorBase {
    static constexpr inline std::string_view identifier{"Intermediate Eigenvector"};
};

struct MajorEigenVector : VectorBase {
    static constexpr inline std::string_view identifier{"Major Eigenvector"};
};

// clang-format off
using types =
    std::tuple<MajorEigenValue,
               IntermediateEigenValue,
               MinorEigenValue,
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
               FrobeniusNorm,
               MinorEigenVector,
               IntermediateEigenVector,
               MinorEigenVector>;
// clang-format on

}  // namespace attributes
}  // namespace inviwo