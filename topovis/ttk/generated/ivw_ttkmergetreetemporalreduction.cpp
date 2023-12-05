#include "ivw_ttkmergetreetemporalreduction.h"

#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/ttk/processors/ttkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataObject.h>
#include <ttkMergeTreeTemporalReduction.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkMergeTreeTemporalReduction& filter) {
        filter.SetAssignmentSolver(property.get());
        return true;
    }
    OptionPropertyInt property{"AssignmentSolver",
                               "Assignment Solver",
                               R"(The assignment solver used in the algorithm.)"_help,
                               {{"Auction", "Auction", 0},
                                {"Exhaustive Search", "Exhaustive Search", 1},
                                {"Munkres", "Munkres", 2}},
                               0};
};

struct Wrapper1 {
    bool set(ttkMergeTreeTemporalReduction& filter) {
        filter.SetRemovalPercentage(property.get());
        return true;
    }
    DoubleProperty property{"RemovalPercentage",
                            "Removal Percentage",
                            R"(Percentage of the total number of data in input to remove.)"_help,
                            50.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkMergeTreeTemporalReduction& filter) {
        filter.SetUseCustomTimeVariable(property.get());
        return true;
    }
    BoolProperty property{
        "UseCustomTimeVariable", "Use custom time variable",
        R"(Allows to use a custom time variable for each data in input. By default the nth input is considered to be the timestep n.)"_help,
        false};
};

struct Wrapper3 {
    bool set(ttkMergeTreeTemporalReduction& filter) {
        filter.SetUseL2Distance(property.get());
        return true;
    }
    BoolProperty property{
        "UseL2Distance", "Use L2 distance",
        R"(Use the L2 distance and L2 barycenter of the original data to compute the reduction (needs to have the Segmentation output of FTMTree).)"_help,
        false};
};

struct Wrapper4 {
    bool set(ttkMergeTreeTemporalReduction& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(ttkMergeTreeTemporalReduction& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber",
                         "Thread Number",
                         R"(The maximum number of threads.)"_help,
                         1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkMergeTreeTemporalReduction& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel",
                         "Debug Level",
                         R"(Debug level.)"_help,
                         3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkMergeTreeTemporalReduction& filter) {
        filter.SetCompactTriangulationCacheSize(property.get());
        return true;
    }
    DoubleProperty property{"CompactTriangulationCacheSize",
                            "Cache",
                            R"(Set the cache size for the compact triangulation as a
ratio with respect to the total cluster number.)"_help,
                            0.2,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkMergeTreeTemporalReduction& filter) {
        filter.Modified();
        return true;
    }
    ButtonProperty property{"Debug_Execute", "Execute",
                            R"(Executes the filter with the last applied parameters, which is
handy to re-start pipeline execution from a specific element
without changing parameters.)"_help};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkMergeTreeTemporalReduction> {
    static constexpr std::string_view className = "ttkMergeTreeTemporalReduction";
    static constexpr std::string_view identifier = "ttkMergeTreeTemporalReduction";
    static constexpr std::string_view displayName = "TTK MergeTreeTemporalReduction";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkMultiBlockDataSet", 1, R"(Merge trees to process.)"}};
    inline static std::array<OutputData, 2> outports = {
        OutputData{"port0", "Key Frames", 0}, OutputData{"port1", "Reduction Coefficients", 1}};
    inline static std::array<Group, 2> groups = {
        Group{"Input options",
              {"AssignmentSolver", "RemovalPercentage", "UseCustomTimeVariable", "TimeVariableName",
               "UseL2Distance"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
    static constexpr std::string_view doc =
        R"(This filter allows to compute a temporal reduction of a sequence of merge trees.

It greedily removes trees in the sequence that can be accurately reconstructed with the geodesic computation. This iterated removal reveals key frames in the sequence (the trees that are not removed and used to reconstruct the removed trees).

The input of this filter is a vtkMultiBlockDataset. Each block is itself a vtkMultiBlockDataset containing the outputs of the FTMTree filter (the segmentation output is optional).

The output of this filter contains the key frames and the reduction coefficients needed to reconstructs the removed trees. To reconstruct the sequence, call the MergeTreeTemporalReductionDecoding filter.

Related publication:
'Wasserstein Distances, Geodesics and Barycenters of Merge Trees'
Mathieu Pont, Jules Vidal, Julie Delon, Julien Tierny.
Proc. of IEEE VIS 2021.
IEEE Transactions on Visualization and Computer Graphics, 2021

Online examples:
- https://topology-tool-kit.github.io/examples/mergeTreeTemporalReduction/)";
};

void registerttkMergeTreeTemporalReduction(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkMergeTreeTemporalReduction>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
