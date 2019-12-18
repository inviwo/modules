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

struct ScalarBase : AttributeBase {
    using value_type = float;
};
struct VectorBase : AttributeBase {
    using value_type = glm::vec3;
};

struct Lambda1 : ScalarBase {
    static constexpr inline std::string_view identifier{ u8"λ1" };
};

struct Lambda2 : ScalarBase {
    static constexpr inline std::string_view identifier{ u8"λ2" };
};

struct Lambda3 : ScalarBase {
    static constexpr inline std::string_view identifier{ u8"λ3" };
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
    static constexpr inline std::string_view identifier{ "Spherical anisotropy" };
};

struct MinorEigenVector : VectorBase {
    static constexpr inline std::string_view identifier{ "Minor Eigenvector" };
};

struct IntermediateEigenVector : VectorBase {
    static constexpr inline std::string_view identifier{ "Intermediate Eigenvector" };
};

struct MajorEigenVector : VectorBase {
    static constexpr inline std::string_view identifier{"Major Eigenvector"};
};

}  // namespace attributes
}  // namespace inviwo