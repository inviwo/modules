#include "ivwwrapscalarfieldcriticalpoints.h"

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
#include "ttkScalarFieldCriticalPoints.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarFieldNew", "Scalar Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"Withpredefinedoffset", "Force Input Offset Field", false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"OffsetNew", "Input Offset Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetVertexBoundary(property.get());
        return true;
    }
    BoolProperty property{"Withboundarymask", "With boundary mask", true};
};

struct Wrapper4 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetVertexIds(property.get());
        return true;
    }
    BoolProperty property{"Withvertexidentifiers", "With vertex identifiers", true};
};

struct Wrapper5 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetVertexScalars(property.get());
        return true;
    }
    BoolProperty property{"Withvertexscalars", "With vertex scalars", true};
};

struct Wrapper6 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper7 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkScalarFieldCriticalPoints> {
    static constexpr std::string_view identifier = "ttkScalarFieldCriticalPoints";
    static constexpr std::string_view displayName = "TTK ScalarFieldCriticalPoints";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input options", {"ScalarFieldNew", "With predefined offset", "OffsetNew"}},
        Group{"Output options",
              {"With boundary mask", "With vertex identifiers", "With vertex scalars"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
};

void registerttkScalarFieldCriticalPoints(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkScalarFieldCriticalPoints>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
