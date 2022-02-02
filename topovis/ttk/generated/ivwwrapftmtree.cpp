#include "ivwwrapftmtree.h"

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
#include "ttkFTMTree.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkFTMTree& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarField", "Scalar Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkFTMTree& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputOffsetScalarField", "Force Input Offset Scalar Field", false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkFTMTree& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"InputOffsetField", "Input Offset Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkFTMTree& filter) {
        filter.SetTreeType(property.get());
        return true;
    }
    OptionPropertyInt property{"TreeType",
                               "Tree Type",
                               {{"Join Tree", "Join Tree", 0},
                                {"Split Tree", "Split Tree", 1},
                                {"Contour Tree", "Contour Tree", 2}},
                               2};
};

struct Wrapper4 {
    bool set(ttkFTMTree& filter) {
        filter.SetSuperArcSamplingLevel(property.get());
        return true;
    }
    IntProperty property{"SuperArcSamplingLevel", "Arc Sampling", 0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkFTMTree& filter) {
        filter.SetWithNormalize(property.get());
        return true;
    }
    BoolProperty property{"NormalizeId", "Deterministic arc and node identifiers", true};
};

struct Wrapper6 {
    bool set(ttkFTMTree& filter) {
        filter.SetWithAdvStats(property.get());
        return true;
    }
    BoolProperty property{"AdvancedStats", "Advanced Statistics", true};
};

struct Wrapper7 {
    bool set(ttkFTMTree& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper8 {
    bool set(ttkFTMTree& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkFTMTree& filter) {
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
struct TTKTraits<ttkFTMTree> {
    static constexpr std::string_view identifier = "ttkftmtree";
    static constexpr std::string_view displayName = "TTK Merge and Contour Tree (FTM)";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 3> outports = {OutputData{"port0", "Skeleton Nodes", 0},
                                                        OutputData{"port1", "Skeleton Arcs", 1},
                                                        OutputData{"port2", "Segmentation", 2}};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}},
        Group{"Input options",
              {"Scalar Field", "ForceInputOffsetScalarField", "Input Offset Field"}},
        Group{"Output options",
              {"Tree Type", "SuperArcSamplingLevel", "NormalizeId", "AdvancedStats"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
};

void registerttkFTMTree(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkFTMTree>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
