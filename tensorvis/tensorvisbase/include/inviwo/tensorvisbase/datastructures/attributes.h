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

struct I1 : ScalarBase {
    static constexpr inline std::string_view identifier_{"I1"};
};

struct I2 : ScalarBase {
    static constexpr inline std::string_view identifier_{"I2"};
};

struct I3 : ScalarBase {
    static constexpr inline std::string_view identifier_{"I3"};
};

struct J1 : ScalarBase {
    static constexpr inline std::string_view identifier_{"J1"};
};

struct J2 : ScalarBase {
    static constexpr inline std::string_view identifier_{"J2"};
};

struct J3 : ScalarBase {
    static constexpr inline std::string_view identifier_{"J3"};
};

struct LodeAngle : ScalarBase {
    static constexpr inline std::string_view identifier_{"Lode angle"};
};

struct Anisotropy : ScalarBase {
    static constexpr inline std::string_view identifier_{"Anisotropy"};
};

struct LinearAnisotropy : ScalarBase {
    static constexpr inline std::string_view identifier_{"Linear anisotropy"};
};

struct PlanarAnisotropy : ScalarBase {
    static constexpr inline std::string_view identifier_{"Planar anisotropy"};
};

struct MajorEigenVector : VectorBase {
    static constexpr inline std::string_view identifier_{"Major Eigenvector"};
};

}  // namespace attributes
}  // namespace inviwo