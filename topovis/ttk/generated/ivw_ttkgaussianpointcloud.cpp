#include "ivw_ttkgaussianpointcloud.h"

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
#include "ttkGaussianPointCloud.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkGaussianPointCloud& filter) {
        filter.SetNumberOfSamples(property.get());
        return true;
    }
    IntProperty property{"NumberOfSamples", "Number of Samples", 1000,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{1000000, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkGaussianPointCloud& filter) {
        filter.SetDimension(property.get());
        return true;
    }
    OptionPropertyInt property{
        "Dimension", "Dimension", {{"1", "1", 1}, {"2", "2", 2}, {"3", "3", 3}}, 1};
};

struct Wrapper2 {
    bool set(ttkGaussianPointCloud& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper3 {
    bool set(ttkGaussianPointCloud& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkGaussianPointCloud& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkGaussianPointCloud& filter) {
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
struct TTKTraits<ttkGaussianPointCloud> {
    static constexpr std::string_view identifier = "ttkGaussianPointCloud";
    static constexpr std::string_view displayName = "TTK GaussianPointCloud";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Output Options", {"NumberOfSamples", "Dimension"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;
};

void registerttkGaussianPointCloud(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkGaussianPointCloud>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
