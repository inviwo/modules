#include "ivw_ttkconnectedcomponents.h"

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
#include "ttkConnectedComponents.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkConnectedComponents& filter) {
        filter.SetBackgroundThreshold(property.get());
        return true;
    }
    DoubleProperty property{"BackgroundThreshold", "BackgroundThreshold", 0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkConnectedComponents& filter) {
        filter.SetAugmentSegmentationWithComponentSize(property.get());
        return true;
    }
    BoolProperty property{"SegmentationSize", "SegmentationSize", false};
};

struct Wrapper2 {
    bool set(ttkConnectedComponents& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper3 {
    bool set(ttkConnectedComponents& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkConnectedComponents& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkConnectedComponents& filter) {
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
struct TTKTraits<ttkConnectedComponents> {
    static constexpr std::string_view identifier = "ConnectedComponents";
    static constexpr std::string_view displayName = "TTK ConnectedComponents";
    inline static std::array<InputData, 1> inports = {InputData{"Segmentation", "vtkDataSet", 1}};
    inline static std::array<OutputData, 2> outports = {OutputData{"port0", "Segmentation", 0},
                                                        OutputData{"port1", "Components", 1}};
    inline static std::array<Group, 3> groups = {
        Group{"Input options", {"FeatureMask", "BackgroundThreshold"}},
        Group{"Output options", {"SegmentationSize"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;
};

void registerttkConnectedComponents(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkConnectedComponents>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
