#include "ivw_ttkmergetreetemporalreductiondecoding.h"
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/vtk/processors/vtkgenericprocessor.h>
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
#include <ttkMergeTreeTemporalReductionDecoding.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
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
                               R"(The assignment solver used in the algorithm.)"_help,
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
    BoolProperty property{"OutputTrees", "Output Trees", R"(Display trees.)"_help, true};
};

struct Wrapper2 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
        filter.SetDimensionSpacing(property.get());
        return true;
    }
    DoubleProperty property{"DimensionSpacing",
                            "Dimension Spacing",
                            R"(Distance between trees.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
        filter.SetPlanarLayout(property.get());
        return true;
    }
    BoolProperty property{"MergeTreePlanarLayout", "Planar Layout",
                          R"(Display trees in a plane or in the original domain.)"_help, true};
};

struct Wrapper4 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
        filter.SetRescaleTreesIndividually(property.get());
        return true;
    }
    BoolProperty property{
        "RescaleTreesIndividually", "Rescale Trees Individually",
        R"(If enabled, the trees will have the same size, it can be interesting to use to individually analyze trees but the comparison between trees given their size will be biased.)"_help,
        false};
};

struct Wrapper5 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper6 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
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

struct Wrapper7 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
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

struct Wrapper8 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
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

struct Wrapper9 {
    bool set(ttkMergeTreeTemporalReductionDecoding& filter) {
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
struct VTKTraits<ttkMergeTreeTemporalReductionDecoding> {
    static constexpr std::string_view uri = "ttk.ttkMergeTreeTemporalReductionDecoding";
    static constexpr std::string_view className = "ttkMergeTreeTemporalReductionDecoding";
    static constexpr std::string_view identifier = "ttkMergeTreeTemporalReductionDecoding";
    static constexpr std::string_view displayName = "TTK MergeTreeTemporalReductionDecoding";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Key Frames", "vtkMultiBlockDataSet", 1, R"(Merge trees to process.)"},
        InputData{"ReductionCoefficients", "vtkTable", 1,
                  R"(Encoding filter reduction coefficients output.)"}};
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
               Wrapper8, Wrapper9>
        properties;
    static constexpr std::string_view doc =
        R"(This filter allows to compute the reconstruction of a reduced sequence of merge trees.

The input of this filter is the key frames and the reduction coefficients in the output of the MergeTreeTemporalReductionEncoding filter.

The output of this filter contains the key frames and the reconstructed trees.

Related publication:
'Wasserstein Distances, Geodesics and Barycenters of Merge Trees'
Mathieu Pont, Jules Vidal, Julie Delon, Julien Tierny.
Proc. of IEEE VIS 2021.
IEEE Transactions on Visualization and Computer Graphics, 2021

Online examples:
- https://topology-tool-kit.github.io/examples/mergeTreeTemporalReduction/)";
};

void registerttkMergeTreeTemporalReductionDecoding(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkMergeTreeTemporalReductionDecoding>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
