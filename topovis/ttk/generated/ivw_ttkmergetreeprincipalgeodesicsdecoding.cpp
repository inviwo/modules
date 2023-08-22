#include "ivw_ttkmergetreeprincipalgeodesicsdecoding.h"

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
#include <ttkMergeTreePrincipalGeodesicsDecoding.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetOutputInputTrees(property.get());
        return true;
    }
    BoolProperty property{"OutputInputTrees", "Output Input Trees", R"()"_help, false};
};

struct Wrapper1 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetOutputInputTreesSegmentation(property.get());
        return true;
    }
    BoolProperty property{"OutputInputTreesSegmentation", "Output Input Trees Segmentation",
                          R"()"_help, false};
};

struct Wrapper2 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetOutputBarycenter(property.get());
        return true;
    }
    BoolProperty property{"OutputBarycenter", "Output Barycenter", R"()"_help, false};
};

struct Wrapper3 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetReconstructInputTrees(property.get());
        return true;
    }
    BoolProperty property{"ReconstructInputTrees", "Reconstruct Input Trees", R"()"_help, true};
};

struct Wrapper4 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetcomputeReconstructionError_(property.get());
        return true;
    }
    BoolProperty property{"ComputeReconstructionError", "Compute Reconstruction Error", R"()"_help,
                          false};
};

struct Wrapper5 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SettransferInputTreesInformation_(property.get());
        return true;
    }
    BoolProperty property{"TransferInputTreesInformation", "Transfer Input Trees Information",
                          R"()"_help, false};
};

struct Wrapper6 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SettransferBarycenterInformation_(property.get());
        return true;
    }
    BoolProperty property{"TransferBarycenterInformation", "Transfer Barycenter Information",
                          R"()"_help, false};
};

struct Wrapper7 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetConstructGeodesicsTrees(property.get());
        return true;
    }
    BoolProperty property{"ConstructGeodesicsTrees", "Construct Geodesics Trees", R"()"_help,
                          false};
};

struct Wrapper8 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetConstructEllipses(property.get());
        return true;
    }
    BoolProperty property{"ConstructEllipses", "Construct Ellipses", R"()"_help, false};
};

struct Wrapper9 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetConstructRectangle(property.get());
        return true;
    }
    BoolProperty property{"ConstructRectangle", "Construct Rectangle", R"()"_help, false};
};

struct Wrapper10 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetRectangleMultiplier(property.get());
        return true;
    }
    IntProperty property{"RectangleMultiplier",
                         "Rectangle Multiplier",
                         R"(Number of intervals in the geodesic.)"_help,
                         1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper11 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetConstructSurface(property.get());
        return true;
    }
    BoolProperty property{"ConstructSurface", "Construct Surface", R"()"_help, false};
};

struct Wrapper12 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetNumberOfGeodesicsIntervals(property.get());
        return true;
    }
    IntProperty property{"NumberOfGeodesicsIntervals",
                         "Number Of Geodesics Intervals",
                         R"(Number of intervals in the geodesic.)"_help,
                         8,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper13 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetProcessSecondInput(property.get());
        return true;
    }
    BoolProperty property{"ProcessSecondInput", "Process Second Input", R"()"_help, false};
};

struct Wrapper14 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper15 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
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

struct Wrapper16 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
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

struct Wrapper17 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
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

struct Wrapper18 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
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
struct TTKTraits<ttkMergeTreePrincipalGeodesicsDecoding> {
    static constexpr std::string_view className = "ttkMergeTreePrincipalGeodesicsDecoding";
    static constexpr std::string_view identifier = "ttkMergeTreePrincipalGeodesicsDecoding";
    static constexpr std::string_view displayName = "TTK MergeTreePrincipalGeodesicsDecoding";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 5> inports = {
        InputData{"Barycenter", "vtkMultiBlockDataSet", 1, R"(Merge trees to process.)"},
        InputData{"Coefficients", "vtkTable", 1,
                  R"(Projection coefficients of the input trees in the new basis.)"},
        InputData{"Geodesics Vectors", "vtkTable", 1,
                  R"(Vectors defining the principal geodesics.)"},
        InputData{"Correlation Matrix (optional)", "vtkTable", 1,
                  R"(Correlation between branches and geodesics axis.)"},
        InputData{"Input Trees (optional)", "vtkMultiBlockDataSet", 1, R"(Original input trees.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{
            "Output options",
            {"OutputInputTrees", "OutputInputTreesSegmentation", "OutputBarycenter",
             "ReconstructInputTrees", "ComputeReconstructionError", "TransferInputTreesInformation",
             "TransferBarycenterInformation", "ConstructGeodesicsTrees", "ConstructEllipses",
             "ConstructRectangle", "RectangleMultiplier", "ConstructSurface",
             "NumberOfGeodesicsIntervals", "ProcessSecondInput"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14, Wrapper15,
               Wrapper16, Wrapper17, Wrapper18>
        properties;
    static constexpr std::string_view doc =
        R"(This filter reconstructs merge trees (or persistence diagrams) given their coordinates in a basis computed via Principal Geodesic Analysis.

Related publication:

'Principal Geodesic Analysis of Merge Trees (and Persistence Diagrams)'
Mathieu Pont, Jules Vidal, Julien Tierny.
IEEE Transactions on Visualization and Computer Graphics, 2022

Online examples:

- https://topology-tool-kit.github.io/examples/mergeTreePGA/)";
};

void registerttkMergeTreePrincipalGeodesicsDecoding(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkMergeTreePrincipalGeodesicsDecoding>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
