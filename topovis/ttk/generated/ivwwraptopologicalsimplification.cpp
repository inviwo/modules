#include "ivwwraptopologicalsimplification.h"

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
#include "ttkTopologicalSimplification.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkTopologicalSimplification& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarField", "Scalar Field", {}, 0};

    static constexpr std::string_view inport = "Domain";
};

struct Wrapper1 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputOffsetScalarField", "Force Input Offset Field", false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkTopologicalSimplification& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(2, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"InputOffsetField", "Input Offset Field", {}, 0};

    static constexpr std::string_view inport = "Domain";
};

struct Wrapper3 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetForceInputVertexScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputVertexScalarField", "Force Input Vertex ScalarField", false};
};

struct Wrapper4 : FieldSelection {
    bool set(ttkTopologicalSimplification& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"VertexIdentifierField", "Vertex Identifier Field", {}, 0};

    static constexpr std::string_view inport = "Constraints";
};

struct Wrapper5 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetConsiderIdentifierAsBlackList(property.get());
        return true;
    }
    BoolProperty property{"ConsiderIdentifierAsBlackList", "Remove selected extrema", false};
};

struct Wrapper6 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetUseLTS(property.get());
        return true;
    }
    BoolProperty property{"UseLTS", "Use LTS", true};
};

struct Wrapper7 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetAddPerturbation(property.get());
        return true;
    }
    BoolProperty property{"AddPerturbation", "Numerical Perturbation", false};
};

struct Wrapper8 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper9 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(ttkTopologicalSimplification& filter) {
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
struct TTKTraits<ttkTopologicalSimplification> {
    static constexpr std::string_view identifier = "ttkTopologicalSimplification";
    static constexpr std::string_view displayName = "TTK TopologicalSimplification";
    inline static std::array<InputData, 2> inports = {InputData{"Domain", "vtkDataSet", 1},
                                                      InputData{"Constraints", "vtkPointSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}},
        Group{
            "Input options",
            {"Scalar Field", "ForceInputVertexScalarField", "Vertex Identifier Field",
             "ForceInputOffsetScalarField", "Input Offset Field", "ConsiderIdentifierAsBlackList"}},
        Group{"Output options", {"UseLTS", "AddPerturbation"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10>
        properties;
};

void registerttkTopologicalSimplification(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkTopologicalSimplification>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
