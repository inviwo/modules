#include "ivw_ttkplanargraphlayout.h"

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
        filter.SetInputIsAMergeTree(property.get());
        return true;
    }
    BoolProperty property{"MergeTreePlanarLayout", "Input is a Merge Tree", false};
};

struct Wrapper1 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseSequences(property.get());
        return true;
    }
    BoolProperty property{"UseSequences", "Use Sequences", true};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkPlanarGraphLayout& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"SequenceArray", "Sequence Array", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseSizes(property.get());
        return true;
    }
    BoolProperty property{"UseSizes", "Use Sizes", false};
};

struct Wrapper4 : FieldSelection {
    bool set(ttkPlanarGraphLayout& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"SizeArray", "Size Array", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper5 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseBranches(property.get());
        return true;
    }
    BoolProperty property{"UseBranches", "Use Branches", false};
};

struct Wrapper6 : FieldSelection {
    bool set(ttkPlanarGraphLayout& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(2, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"BranchArray", "Branch Array", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper7 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseLevels(property.get());
        return true;
    }
    BoolProperty property{"UseLevels", "Use Levels", false};
};

struct Wrapper8 : FieldSelection {
    bool set(ttkPlanarGraphLayout& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(3, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"LevelArray", "Level Array", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper9 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetOutputArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"OutputArrayName", "Output Array Name", "Layout"};
};

struct Wrapper10 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper11 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper12 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper13 {
    bool set(ttkPlanarGraphLayout& filter) {
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
struct TTKTraits<ttkPlanarGraphLayout> {
    static constexpr std::string_view identifier = "ttkPlanarGraphLayout";
    static constexpr std::string_view displayName = "TTK PlanarGraphLayout";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkUnstructuredGrid", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input Options",
              {"MergeTreePlanarLayout", "UseSequences", "SequenceArray", "UseSizes", "SizeArray",
               "UseBranches", "BranchArray", "UseLevels", "LevelArray"}},
        Group{"Output Options", {"OutputArrayName"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13>
        properties;
};

void registerttkPlanarGraphLayout(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkPlanarGraphLayout>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
