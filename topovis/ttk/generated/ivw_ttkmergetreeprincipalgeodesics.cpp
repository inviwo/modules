#include "ivw_ttkmergetreeprincipalgeodesics.h"
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/vtk/processors/vtkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/ttk/util/ttkprocessorutils.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataObject.h>
#include <ttkMergeTreePrincipalGeodesics.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetNormalizedWasserstein(property.get());
        return true;
    }
    BoolProperty property{"NormalizedWasserstein", "Normalized Wasserstein", R"()"_help, true};
};

struct Wrapper1 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetNumberOfGeodesics(property.get());
        return true;
    }
    IntProperty property{"NumberOfGeodesics",
                         "Number Of Geodesics",
                         R"(Number of geodesics to compute.)"_help,
                         2,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetNumberOfProjectionIntervals(property.get());
        return true;
    }
    IntProperty property{
        "NumberOfProjectionIntervals",
        "Number Of Projection Intervals",
        R"(Number of intervals in the geodesic to compute the projection of each input.)"_help,
        16,
        std::pair{0, ConstraintBehavior::Ignore},
        std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetNumberOfProjectionSteps(property.get());
        return true;
    }
    IntProperty property{"NumberOfProjectionSteps",
                         "Number Of Projection Steps",
                         R"()"_help,
                         8,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetBarycenterSizeLimitPercent(property.get());
        return true;
    }
    DoubleProperty property{"BarycenterSizeLimitPercent",
                            "Barycenter Size Limit Percent",
                            R"()"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetDeterministic(property.get());
        return true;
    }
    BoolProperty property{"Deterministic", "Deterministic", R"()"_help, false};
};

struct Wrapper6 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetDiagramPairTypes(property.get());
        return true;
    }
    OptionPropertyInt property{
        "DiagramPairTypes",
        "Diagram Pair Types",
        R"(The types of pairs to use when the input are persistence diagrams. This can be weigthed with the "Pair Type Mixture Coefficient" parameter (0 for only the second type of pairs and 1 for the first type of pairs).)"_help,
        {{"Min-Saddle and Saddle-Max", "Min-Saddle and Saddle-Max", 0},
         {"Min-Saddle and Saddle-Saddle", "Min-Saddle and Saddle-Saddle", 1},
         {"Saddle-Saddle Saddle-Max", "Saddle-Saddle Saddle-Max", 2}},
        0};
};

struct Wrapper7 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetJoinSplitMixtureCoefficient(property.get());
        return true;
    }
    DoubleProperty property{
        "JoinSplitMixtureCoefficient",
        "Pair Type Mixture Coefficient",
        R"(If input are merge trees, this parameter allows to weight between the first input and the second input, typically join and split trees (0 for only the second input and 1 for only the first one).
If input are persistence diagrams, this parameter allows to weight between different types of pairs, determined by the "Diagram Pair Types" parameter (default is min-sad and sad-max pairs), 0 for only the second type and 1 for only the first type.)"_help,
        0.5,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetComputeReconstructionError(property.get());
        return true;
    }
    BoolProperty property{"ComputeReconstructionError", "Compute Reconstruction Error", R"()"_help,
                          false};
};

struct Wrapper9 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper10 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
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

struct Wrapper11 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
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

struct Wrapper12 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
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

struct Wrapper13 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
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
struct VTKTraits<ttkMergeTreePrincipalGeodesics> {
    static constexpr std::string_view uri = "ttk.ttkMergeTreePrincipalGeodesics";
    static constexpr std::string_view className = "ttkMergeTreePrincipalGeodesics";
    static constexpr std::string_view identifier = "ttkMergeTreePrincipalGeodesics";
    static constexpr std::string_view displayName = "TTK MergeTreePrincipalGeodesics";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 2> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkMultiBlockDataSet",
                  .numComp = 1,
                  .doc = R"(Merge trees to process.)"},
        InputData{
            .identifier = "Optional Input",
            .dataType = "vtkMultiBlockDataSet",
            .numComp = 1,
            .doc =
                R"(If input are merge trees, then this input can be used to process join and split trees together. Pass as input either join or split trees in the first input and the other type of trees in the second input.
If input are persistence diagrams, then this has no effect to use this input.)"}};
    inline static std::array<OutputData, 4> outports = {
        OutputData{"port0", "Barycenter", 0}, OutputData{"port1", "Coefficients", 1},
        OutputData{"port2", "Geodesics Vectors", 2}, OutputData{"port3", "Correlation Matrix", 3}};
    inline static std::array<Group, 3> groups = {
        Group{"Input options",
              {"NormalizedWasserstein", "NumberOfGeodesics", "NumberOfProjectionIntervals",
               "NumberOfProjectionSteps", "BarycenterSizeLimitPercent", "Deterministic",
               "DiagramPairTypes", "JoinSplitMixtureCoefficient"}},
        Group{"Output options", {"ComputeReconstructionError"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This filter computes Principal Geodesic Analysis on the space of merge trees or persistence diagrams, that is, a set of orthogonal geodesic axes defining an optimized basis with the barycenter as origin.

Related publication:

'Principal Geodesic Analysis of Merge Trees (and Persistence Diagrams)'
Mathieu Pont, Jules Vidal, Julien Tierny.
IEEE Transactions on Visualization and Computer Graphics, 2022

Online examples:

- https://topology-tool-kit.github.io/examples/mergeTreePGA/

- https://topology-tool-kit.github.io/examples/persistenceDiagramPGA/)ivw";
};

void registerttkMergeTreePrincipalGeodesics(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkMergeTreePrincipalGeodesics>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
