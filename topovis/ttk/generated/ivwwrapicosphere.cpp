#include "ivwwrapicosphere.h"

#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/ttk/processors/ttkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include "ttkIcosphere.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkIcosphere& filter) {
        filter.SetNumberOfSubdivisions(property.get());
        return true;
    }
    IntProperty property{"NumberOfSubdivisions", "Subdivisions", 0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkIcosphere& filter) {
        filter.SetRadius(property.get());
        return true;
    }
    DoubleProperty property{"Radius", "Radius", 1.0, std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkIcosphere& filter) {
        filter.SetCenter(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{"Center", "Center", dvec3{0.0, 0.0, 0.0},
                                std::pair{dvec3{0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec3{100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkIcosphere& filter) {
        filter.SetComputeNormals(property.get());
        return true;
    }
    BoolProperty property{"ComputeNormals", "Compute Normals", false};
};

struct Wrapper4 {
    bool set(ttkIcosphere& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper5 {
    bool set(ttkIcosphere& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkIcosphere& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkIcosphere& filter) {
        filter.SetCompactTriangulationCacheSize(property.get());
        return true;
    }
    DoubleProperty property{"CompactTriangulationCacheSize", "Cache", 0.2,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkIcosphere> {
    static constexpr std::string_view identifier = "ttkIcosphere";
    static constexpr std::string_view displayName = "TTK Icosphere";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Output Options", {"NumberOfSubdivisions", "Radius", "Center", "ComputeNormals"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
};

void registerttkIcosphere(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkIcosphere>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
