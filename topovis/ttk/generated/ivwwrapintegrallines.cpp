#include "ivwwrapintegrallines.h"

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
#include "ttkIntegralLines.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkIntegralLines& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarFieldNew", "Scalar Field", {}, 0};

    static constexpr std::string_view inport = "Domain";
};

struct Wrapper1 {
    bool set(ttkIntegralLines& filter) {
        filter.SetDirection(property.get());
        return true;
    }
    OptionPropertyInt property{
        "Direction", "Direction", {{"Forward", "Forward", 0}, {"Backward", "Backward", 1}}, 0};
};

struct Wrapper2 {
    bool set(ttkIntegralLines& filter) {
        filter.SetForceInputVertexScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputVertexScalarField", "Force Input Vertex ScalarField", false};
};

struct Wrapper3 : FieldSelection {
    bool set(ttkIntegralLines& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(2, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"InputVertexScalarField", "Vertex identifier field", {}, 0};

    static constexpr std::string_view inport = "Seeds";
};

struct Wrapper4 {
    bool set(ttkIntegralLines& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputOffsetScalarField", "Force Input Offset Field", false};
};

struct Wrapper5 : FieldSelection {
    bool set(ttkIntegralLines& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"OffsetScalarField", "Input Offset Field", {}, 0};

    static constexpr std::string_view inport = "Domain";
};

struct Wrapper6 {
    bool set(ttkIntegralLines& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper7 {
    bool set(ttkIntegralLines& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkIntegralLines& filter) {
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
struct TTKTraits<ttkIntegralLines> {
    static constexpr std::string_view identifier = "ttkIntegralLines";
    static constexpr std::string_view displayName = "TTK IntegralLines";
    inline static std::array<InputData, 2> inports = {InputData{"Domain", "vtkDataSet", 1},
                                                      InputData{"Seeds", "vtkPointSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}},
        Group{"Input options",
              {"ScalarFieldNew", "Direction", "ForceInputVertexScalarField",
               "InputVertexScalarField", "ForceInputOffsetScalarField", "OffsetScalarField"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
};

void registerttkIntegralLines(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkIntegralLines>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
