#include "ivwwrappersistencediagram.h"

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
#include "ttkPersistenceDiagram.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkPersistenceDiagram& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarFieldNew", "Scalar Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputOffsetScalarField", "Force Input Offset Field", false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkPersistenceDiagram& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"InputOffsetScalarFieldNameNew", "Input Offset Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetComputeSaddleConnectors(property.get());
        return true;
    }
    BoolProperty property{"SaddleConnectors", "Compute saddle-saddle pairs (SLOW!)", false};
};

struct Wrapper4 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetShowInsideDomain(property.get());
        return true;
    }
    BoolProperty property{"ShowInsideDomain", "Embed in Domain", false};
};

struct Wrapper5 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper6 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkPersistenceDiagram& filter) {
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
struct TTKTraits<ttkPersistenceDiagram> {
    static constexpr std::string_view identifier = "ttkPersistenceDiagram";
    static constexpr std::string_view displayName = "TTK PersistenceDiagram";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input options",
              {"ScalarFieldNew", "ForceInputOffsetScalarField", "InputOffsetScalarFieldNameNew"}},
        Group{"Output options", {"SaddleConnectors", "ShowInsideDomain"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
};

void registerttkPersistenceDiagram(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkPersistenceDiagram>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
