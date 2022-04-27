#include "ivwwrapmandatorycriticalpoints.h"

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
#include "ttkMandatoryCriticalPoints.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkMandatoryCriticalPoints& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"LowerBoundField", "Lower Bound Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 : FieldSelection {
    bool set(ttkMandatoryCriticalPoints& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"UpperBoundField", "Upper Bound Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.SetSimplificationThreshold(property.get());
        return true;
    }
    DoubleProperty property{"SimplificationThreshold", "Normalized Threshold", 0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.setOutputAllMinimumComponents(property.get());
        return true;
    }
    BoolProperty property{"OutputAllMinimumComponents", "Display all minima", true};
};

struct Wrapper4 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.SetOutputMinimumComponentId(property.get());
        return true;
    }
    IntProperty property{"OutputMinmumComponentId", "Only display minimum with Id", 0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.setOutputAllJoinSaddleComponents(property.get());
        return true;
    }
    BoolProperty property{"OutputAllJoinSaddleComponents", "Display all join saddles", true};
};

struct Wrapper6 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.SetOutputJoinSaddleComponentId(property.get());
        return true;
    }
    IntProperty property{"OutputJoinSaddleComponentId", "Only display join saddle with Id", 0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.setOutputAllSplitSaddleComponents(property.get());
        return true;
    }
    BoolProperty property{"OutputAllSplitSaddleComponents", "Display all split saddles", true};
};

struct Wrapper8 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.SetOutputSplitSaddleComponentId(property.get());
        return true;
    }
    IntProperty property{"OutputSplitSaddleComponentId", "Only display split saddle with Id", 0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.setOutputAllMaximumComponents(property.get());
        return true;
    }
    BoolProperty property{"OutputAllMaximumComponents", "Display all maxima", true};
};

struct Wrapper10 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.SetOutputMaximumComponentId(property.get());
        return true;
    }
    IntProperty property{"OutputMaxmumComponentId", "Only display maximum with Id", 0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper11 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper12 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper13 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper14 {
    bool set(ttkMandatoryCriticalPoints& filter) {
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
struct TTKTraits<ttkMandatoryCriticalPoints> {
    static constexpr std::string_view identifier = "ttkMandatoryCriticalPoints";
    static constexpr std::string_view displayName = "TTK MandatoryCriticalPoints";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 6> outports = {
        OutputData{"port0", "MandatoryMinima", 0},
        OutputData{"port1", "MandatoryJoinSaddles", 1},
        OutputData{"port2", "MandatorySplitSaddles", 2},
        OutputData{"port3", "MandatoryMaxima", 3},
        OutputData{"port4", "MandatoryJoinTree", 4},
        OutputData{"port5", "MandatorySplitTree", 5}};
    inline static std::array<Group, 4> groups = {
        Group{"Simplification", {"SimplificationThreshold"}},
        Group{"Output options",
              {"OutputAllMinimumComponents", "OutputAllJoinSaddleComponents",
               "OutputAllSplitSaddleComponents", "OutputAllMaximumComponents",
               "OutputMinmumComponentId", "OutputJoinSaddleComponentId",
               "OutputSplitSaddleComponentId", "OutputMaxmumComponentId"}},
        Group{"Input options",
              {"SimplificationThreshold", "Lower Bound Field", "Upper Bound Field"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14>
        properties;
};

void registerttkMandatoryCriticalPoints(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkMandatoryCriticalPoints>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
