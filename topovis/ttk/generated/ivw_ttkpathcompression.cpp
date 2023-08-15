#include "ivw_ttkpathcompression.h"

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
#include "ttkPathCompression.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkPathCompression& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarField", "Scalar Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkPathCompression& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputOffsetScalarField", "Force Input Offset Field", false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkPathCompression& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"OffsetField", "Offset Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkPathCompression& filter) {
        filter.SetComputeAscendingSegmentation(property.get());
        return true;
    }
    BoolProperty property{"ComputeAscendingSegmentation", "Ascending Segmentation", true};
};

struct Wrapper4 {
    bool set(ttkPathCompression& filter) {
        filter.SetComputeDescendingSegmentation(property.get());
        return true;
    }
    BoolProperty property{"ComputeDescendingSegmentation", "Descending Segmentation", true};
};

struct Wrapper5 {
    bool set(ttkPathCompression& filter) {
        filter.SetComputeMSSegmentationHash(property.get());
        return true;
    }
    BoolProperty property{"ComputeFinalSegmentation", "Morse-Smale Complex Hash", true};
};

struct Wrapper6 {
    bool set(ttkPathCompression& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper7 {
    bool set(ttkPathCompression& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkPathCompression& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkPathCompression& filter) {
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
struct TTKTraits<ttkPathCompression> {
    static constexpr std::string_view identifier = "ttkPathCompression";
    static constexpr std::string_view displayName = "TTK PathCompression";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 1> outports = {OutputData{"port0", "Segmentation", 0}};
    inline static std::array<Group, 3> groups = {
        Group{"Input options", {"Scalar Field", "ForceInputOffsetScalarField", "Offset Field"}},
        Group{"Output options",
              {"ComputeAscendingSegmentation", "ComputeDescendingSegmentation",
               "ComputeFinalSegmentation"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
};

void registerttkPathCompression(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkPathCompression>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
