#include "ivw_ttkmergetreeclustering.h"

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
#include "ttkMergeTreeClustering.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetComputeBarycenter(property.get());
        return true;
    }
    BoolProperty property{"ComputeBarycenter", "Compute Barycenter", false};
};

struct Wrapper1 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetNumberOfBarycenters(property.get());
        return true;
    }
    IntProperty property{"NumberOfBarycenters", "Number Of Clusters", 1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetDeterministic(property.get());
        return true;
    }
    BoolProperty property{"Deterministic", "Deterministic", false};
};

struct Wrapper3 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetAlpha(property.get());
        return true;
    }
    DoubleProperty property{"Alpha", "Alpha", 0.5, std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetJoinSplitMixtureCoefficient(property.get());
        return true;
    }
    DoubleProperty property{"JoinSplitMixtureCoefficient", "Join Split Mixture Coefficient", 0.5,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetOutputTrees(property.get());
        return true;
    }
    BoolProperty property{"OutputTrees", "Output Trees", true};
};

struct Wrapper6 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetDimensionSpacing(property.get());
        return true;
    }
    DoubleProperty property{"DimensionSpacing", "Dimension Spacing", 1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetDimensionToShift(property.get());
        return true;
    }
    OptionPropertyInt property{
        "DimensionToShift", "Dimension To shift", {{"X", "X", 0}, {"Y", "Y", 1}, {"Z", "Z", 2}}, 0};
};

struct Wrapper8 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetBarycenterPositionAlpha(property.get());
        return true;
    }
    BoolProperty property{"BarycenterPositionAlpha", "Barycenter position according to alpha",
                          false};
};

struct Wrapper9 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetPlanarLayout(property.get());
        return true;
    }
    BoolProperty property{"MergeTreePlanarLayout", "Planar Layout", true};
};

struct Wrapper10 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetRescaleTreesIndividually(property.get());
        return true;
    }
    BoolProperty property{"RescaleTreesIndividually", "Rescale Trees Individually", false};
};

struct Wrapper11 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetOutputSegmentation(property.get());
        return true;
    }
    BoolProperty property{"OutputSegmentation", "Output Segmentation", false};
};

struct Wrapper12 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper13 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper14 {
    bool set(ttkMergeTreeClustering& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper15 {
    bool set(ttkMergeTreeClustering& filter) {
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
struct TTKTraits<ttkMergeTreeClustering> {
    static constexpr std::string_view identifier = "MergeTreeClustering";
    static constexpr std::string_view displayName = "TTK MergeTreeClustering";
    inline static std::array<InputData, 2> inports = {
        InputData{"Input", "vtkMultiBlockDataSet", 1},
        InputData{"Optional Input (clustering)", "vtkMultiBlockDataSet", 1}};
    inline static std::array<OutputData, 3> outports = {
        OutputData{"port0", "Clustered Merge Trees", 0},
        OutputData{"port1", "Centroids Merge Trees", 1}, OutputData{"port2", "Matchings", 2}};
    inline static std::array<Group, 3> groups = {
        Group{"Input options",
              {"ComputeBarycenter", "NumberOfBarycenters", "Deterministic", "Alpha",
               "JoinSplitMixtureCoefficient"}},
        Group{"Output options",
              {"OutputTrees", "DimensionSpacing", "DimensionToShift", "BarycenterPositionAlpha",
               "MergeTreePlanarLayout", "RescaleTreesIndividually", "OutputSegmentation"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14, Wrapper15>
        properties;
};

void registerttkMergeTreeClustering(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkMergeTreeClustering>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
