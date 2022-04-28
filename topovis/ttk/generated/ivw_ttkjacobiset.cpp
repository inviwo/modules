#include "ivw_ttkjacobiset.h"

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
#include "ttkJacobiSet.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkJacobiSet& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"UComponent", "U Component", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 : FieldSelection {
    bool set(ttkJacobiSet& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"VComponent", "V Component", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(ttkJacobiSet& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"Withpredefinedoffset", "Force Input Offset Field", false};
};

struct Wrapper3 : FieldSelection {
    bool set(ttkJacobiSet& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(2, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"UOffsetField", "U Offset Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper4 : FieldSelection {
    bool set(ttkJacobiSet& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(3, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"VOffsetField", "V Offset Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper5 {
    bool set(ttkJacobiSet& filter) {
        filter.SetEdgeIds(property.get());
        return true;
    }
    BoolProperty property{"Withedgeidentifiers", "With edge identifiers", false};
};

struct Wrapper6 {
    bool set(ttkJacobiSet& filter) {
        filter.SetVertexScalars(property.get());
        return true;
    }
    BoolProperty property{"Withvertexscalars", "With vertex scalars", false};
};

struct Wrapper7 {
    bool set(ttkJacobiSet& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper8 {
    bool set(ttkJacobiSet& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkJacobiSet& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(ttkJacobiSet& filter) {
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
struct TTKTraits<ttkJacobiSet> {
    static constexpr std::string_view identifier = "ttkJacobiSet";
    static constexpr std::string_view displayName = "TTK JacobiSet";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options",
              {"U Component", "V Component", "With predefined offset", "U Offset Field",
               "V Offset Field"}},
        Group{"Output options", {"With edge identifiers", "With vertex scalars"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10>
        properties;
};

void registerttkJacobiSet(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkJacobiSet>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
