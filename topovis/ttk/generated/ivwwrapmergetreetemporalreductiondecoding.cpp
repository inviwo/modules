#include "ivwwrapmergetreetemporalreductiondecoding.h"

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
#include "ttkMergeTreeTemporalReductionDecoding.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
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
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
        filter.SetOutputTrees(property.get());
        return true;
    }
    BoolProperty property{"OutputTrees", "Output Trees", true};
};

struct Wrapper2 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
        filter.SetDimensionSpacing(property.get());
        return true;
    }
    DoubleProperty property{"DimensionSpacing", "Dimension Spacing", 1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
        filter.SetPlanarLayout(property.get());
        return true;
    }
    BoolProperty property{"MergeTreePlanarLayout", "Planar Layout", true};
};

struct Wrapper4 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
        filter.SetRescaleTreesIndividually(property.get());
        return true;
    }
    BoolProperty property{"RescaleTreesIndividually", "Rescale Trees Individually", false};
};

struct Wrapper5 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper6 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
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
struct TTKTraits<ttkMergeTreeTemporalReductionDecoding> {
    static constexpr std::string_view identifier = "ttkMergeTreeTemporalReductionDecoding";
    static constexpr std::string_view displayName = "TTK MergeTreeTemporalReductionDecoding";
    inline static std::array<InputData, 2> inports = {
        InputData{"Key Frames", "vtkMultiBlockDataSet", 1},
        InputData{"ReductionCoefficients", "vtkTable", 1}};
    inline static std::array<OutputData, 2> outports = {
        OutputData{"port0", "Reconstructed Sequence", 0}, OutputData{"port1", "Matching", 1}};
    inline static std::array<Group, 3> groups = {
        Group{"Input options", {"AssignmentSolver"}},
        Group{"Output options",
              {"OutputTrees", "DimensionSpacing", "MergeTreePlanarLayout",
               "RescaleTreesIndividually"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
};

void registerttkMergeTreeTemporalReductionDecoding(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkMergeTreeTemporalReductionDecoding>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo