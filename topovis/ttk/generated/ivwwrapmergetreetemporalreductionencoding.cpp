#include "ivwwrapmergetreetemporalreductionencoding.h"

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
#include "ttkMergeTreeTemporalReductionEncoding.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkMergeTreeTemporalReductionEncoding& filter) {
        filter.SetAssignmentSolver(property.get());
        return true;
    }
    OptionPropertyInt property{"AssignmentSolver",
                               "Assignment Solver",
                               {{"Auction", "Auction", 0},
                                {"Exhaustive Search", "Exhaustive Search", 1},
                                {"Munkres", "Munkres", 2}},
                               0};
};

struct Wrapper1 {
    bool set(ttkMergeTreeTemporalReductionEncoding& filter) {
        filter.SetRemovalPercentage(property.get());
        return true;
    }
    DoubleProperty property{"RemovalPercentage", "Removal Percentage", 50.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkMergeTreeTemporalReductionEncoding& filter) {
        filter.SetUseCustomTimeVariable(property.get());
        return true;
    }
    BoolProperty property{"UseCustomTimeVariable", "Use custom time variable", false};
};

struct Wrapper3 {
    bool set(ttkMergeTreeTemporalReductionEncoding& filter) {
        filter.SetUseL2Distance(property.get());
        return true;
    }
    BoolProperty property{"UseL2Distance", "Use L2 distance", false};
};

struct Wrapper4 {
    bool set(ttkMergeTreeTemporalReductionEncoding& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper5 {
    bool set(ttkMergeTreeTemporalReductionEncoding& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkMergeTreeTemporalReductionEncoding& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkMergeTreeTemporalReductionEncoding& filter) {
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
struct TTKTraits<ttkMergeTreeTemporalReductionEncoding> {
    static constexpr std::string_view identifier = "ttkMergeTreeTemporalReductionEncoding";
    static constexpr std::string_view displayName = "TTK MergeTreeTemporalReductionEncoding";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkMultiBlockDataSet", 1}};
    inline static std::array<OutputData, 2> outports = {
        OutputData{"port0", "Key Frames", 0}, OutputData{"port1", "Reduction Coefficients", 1}};
    inline static std::array<Group, 2> groups = {
        Group{"Input options",
              {"AssignmentSolver", "RemovalPercentage", "UseCustomTimeVariable", "TimeVariableName",
               "UseL2Distance"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
};

void registerttkMergeTreeTemporalReductionEncoding(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkMergeTreeTemporalReductionEncoding>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
