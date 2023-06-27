#include "ivw_ttkmergetreeprincipalgeodesicsdecoding.h"

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
#include "ttkMergeTreePrincipalGeodesicsDecoding.h"
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
    BoolProperty property{"OutputInputTrees", "Output Input Trees", false};
};

struct Wrapper1 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetOutputInputTreesSegmentation(property.get());
        return true;
    }
    BoolProperty property{"OutputInputTreesSegmentation", "Output Input Trees Segmentation", false};
};

struct Wrapper2 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetOutputBarycenter(property.get());
        return true;
    }
    BoolProperty property{"OutputBarycenter", "Output Barycenter", false};
};

struct Wrapper3 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetReconstructInputTrees(property.get());
        return true;
    }
    BoolProperty property{"ReconstructInputTrees", "Reconstruct Input Trees", true};
};

struct Wrapper4 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetcomputeReconstructionError_(property.get());
        return true;
    }
    BoolProperty property{"ComputeReconstructionError", "Compute Reconstruction Error", false};
};

struct Wrapper5 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SettransferInputTreesInformation_(property.get());
        return true;
    }
    BoolProperty property{"TransferInputTreesInformation", "Transfer Input Trees Information",
                          false};
};

struct Wrapper6 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SettransferBarycenterInformation_(property.get());
        return true;
    }
    BoolProperty property{"TransferBarycenterInformation", "Transfer Barycenter Information",
                          false};
};

struct Wrapper7 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetConstructGeodesicsTrees(property.get());
        return true;
    }
    BoolProperty property{"ConstructGeodesicsTrees", "Construct Geodesics Trees", false};
};

struct Wrapper8 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetConstructEllipses(property.get());
        return true;
    }
    BoolProperty property{"ConstructEllipses", "Construct Ellipses", false};
};

struct Wrapper9 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetConstructRectangle(property.get());
        return true;
    }
    BoolProperty property{"ConstructRectangle", "Construct Rectangle", false};
};

struct Wrapper10 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetRectangleMultiplier(property.get());
        return true;
    }
    IntProperty property{"RectangleMultiplier", "Rectangle Multiplier", 1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper11 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetConstructSurface(property.get());
        return true;
    }
    BoolProperty property{"ConstructSurface", "Construct Surface", false};
};

struct Wrapper12 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetNumberOfGeodesicsIntervals(property.get());
        return true;
    }
    IntProperty property{"NumberOfGeodesicsIntervals", "Number Of Geodesics Intervals", 8,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper13 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetProcessSecondInput(property.get());
        return true;
    }
    BoolProperty property{"ProcessSecondInput", "Process Second Input", false};
};

struct Wrapper14 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper15 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper16 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper17 {
    bool set(ttkMergeTreePrincipalGeodesicsDecoding& filter) {
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
struct TTKTraits<ttkMergeTreePrincipalGeodesicsDecoding> {
    static constexpr std::string_view identifier = "ttkMergeTreePrincipalGeodesicsDecoding";
    static constexpr std::string_view displayName = "TTK MergeTreePrincipalGeodesicsDecoding";
    inline static std::array<InputData, 5> inports = {
        InputData{"Barycenter", "vtkMultiBlockDataSet", 1},
        InputData{"Coefficients", "vtkTable", 1}, InputData{"Geodesics Vectors", "vtkTable", 1},
        InputData{"Correlation Matrix (optional)", "vtkTable", 1},
        InputData{"Input Trees (optional)", "vtkMultiBlockDataSet", 1}};
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
               Wrapper16, Wrapper17>
        properties;
};

void registerttkMergeTreePrincipalGeodesicsDecoding(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkMergeTreePrincipalGeodesicsDecoding>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
