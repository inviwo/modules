#include "ivw_ttkmergetreeprincipalgeodesics.h"

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
#include "ttkMergeTreePrincipalGeodesics.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetNormalizedWasserstein(property.get());
        return true;
    }
    BoolProperty property{"NormalizedWasserstein", "Normalized Wasserstein", true};
};

struct Wrapper1 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetNumberOfGeodesics(property.get());
        return true;
    }
    IntProperty property{"NumberOfGeodesics", "Number Of Geodesics", 2,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetNumberOfProjectionIntervals(property.get());
        return true;
    }
    IntProperty property{"NumberOfProjectionIntervals", "Number Of Projection Intervals", 16,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetNumberOfProjectionSteps(property.get());
        return true;
    }
    IntProperty property{"NumberOfProjectionSteps", "Number Of Projection Steps", 8,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetBarycenterSizeLimitPercent(property.get());
        return true;
    }
    DoubleProperty property{"BarycenterSizeLimitPercent", "Barycenter Size Limit Percent", 0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetDeterministic(property.get());
        return true;
    }
    BoolProperty property{"Deterministic", "Deterministic", false};
};

struct Wrapper6 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetJoinSplitMixtureCoefficient(property.get());
        return true;
    }
    DoubleProperty property{"JoinSplitMixtureCoefficient", "Pair Type Mixture Coefficient", 0.5,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetComputeReconstructionError(property.get());
        return true;
    }
    BoolProperty property{"ComputeReconstructionError", "Compute Reconstruction Error", false};
};

struct Wrapper8 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper9 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper11 {
    bool set(ttkMergeTreePrincipalGeodesics& filter) {
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
struct TTKTraits<ttkMergeTreePrincipalGeodesics> {
    static constexpr std::string_view identifier = "ttkMergeTreePrincipalGeodesics";
    static constexpr std::string_view displayName = "TTK MergeTreePrincipalGeodesics";
    inline static std::array<InputData, 2> inports = {
        InputData{"Input", "vtkMultiBlockDataSet", 1},
        InputData{"Optional Input", "vtkMultiBlockDataSet", 1}};
    inline static std::array<OutputData, 4> outports = {
        OutputData{"port0", "Barycenter", 0}, OutputData{"port1", "Coefficients", 1},
        OutputData{"port2", "Geodesics Vectors", 2}, OutputData{"port3", "Correlation Matrix", 3}};
    inline static std::array<Group, 3> groups = {
        Group{"Input options",
              {"NormalizedWasserstein", "NumberOfGeodesics", "NumberOfProjectionIntervals",
               "NumberOfProjectionSteps", "BarycenterSizeLimitPercent", "Deterministic",
               "JoinSplitMixtureCoefficient"}},
        Group{"Output options", {"ComputeReconstructionError"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11>
        properties;
};

void registerttkMergeTreePrincipalGeodesics(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkMergeTreePrincipalGeodesics>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
