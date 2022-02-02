#include "ivwwrapplanargraphlayout.h"

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
#include "ttkPlanarGraphLayout.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseSequences(property.get());
        return true;
    }
    BoolProperty property{"UseSequences", "Use Sequences", true};
};

struct Wrapper1 : FieldSelection {
    bool set(ttkPlanarGraphLayout& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"SequenceArray", "Sequence Array", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseSizes(property.get());
        return true;
    }
    BoolProperty property{"UseSizes", "Use Sizes", false};
};

struct Wrapper3 : FieldSelection {
    bool set(ttkPlanarGraphLayout& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"SizeArray", "Size Array", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper4 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseBranches(property.get());
        return true;
    }
    BoolProperty property{"UseBranches", "Use Branches", false};
};

struct Wrapper5 : FieldSelection {
    bool set(ttkPlanarGraphLayout& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(2, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"BranchArray", "Branch Array", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper6 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseLevels(property.get());
        return true;
    }
    BoolProperty property{"UseLevels", "Use Levels", false};
};

struct Wrapper7 : FieldSelection {
    bool set(ttkPlanarGraphLayout& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(3, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"LevelArray", "Level Array", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper8 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetOutputArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"OutputArrayName", "Output Array Name", "Layout"};
};

struct Wrapper9 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper10 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper11 {
    bool set(ttkPlanarGraphLayout& filter) {
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
struct TTKTraits<ttkPlanarGraphLayout> {
    static constexpr std::string_view identifier = "ttkPlanarGraphLayout";
    static constexpr std::string_view displayName = "TTK PlanarGraphLayout";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkUnstructuredGrid", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input Options",
              {"UseSequences", "SequenceArray", "UseSizes", "SizeArray", "UseBranches",
               "BranchArray", "UseLevels", "LevelArray"}},
        Group{"Output Options", {"OutputArrayName"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11>
        properties;
};

void registerttkPlanarGraphLayout(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkPlanarGraphLayout>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
