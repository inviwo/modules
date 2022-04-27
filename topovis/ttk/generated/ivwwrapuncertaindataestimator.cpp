#include "ivwwrapuncertaindataestimator.h"

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
#include "ttkUncertainDataEstimator.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkUncertainDataEstimator& filter) {
        filter.SetBoundToCompute(property.get());
        return true;
    }
    OptionPropertyInt property{"BoundtoCompute",
                               "Bound to Compute",
                               {{"Lower and Upper", "Lower and Upper", 0},
                                {"Lower only", "Lower only", 1},
                                {"Upper only", "Upper only", 2}},
                               0};
};

struct Wrapper1 {
    bool set(ttkUncertainDataEstimator& filter) {
        filter.SetBinCount(property.get());
        return true;
    }
    IntProperty property{"BinCount", "Bin count", 10, std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{1000, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkUncertainDataEstimator& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper3 {
    bool set(ttkUncertainDataEstimator& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkUncertainDataEstimator& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkUncertainDataEstimator& filter) {
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
struct TTKTraits<ttkUncertainDataEstimator> {
    static constexpr std::string_view identifier = "ttkUncertainDataEstimator";
    static constexpr std::string_view displayName = "TTK UncertainDataEstimator";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 3> outports = {OutputData{"port0", "Bound Fields", 0},
                                                        OutputData{"port1", "Probabilities", 1},
                                                        OutputData{"port2", "MeanField", 2}};
    inline static std::array<Group, 4> groups = {
        Group{"Bound Fields", {"Bound to Compute"}},
        Group{"Probability Density Functions", {"BinCount"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options", {"Bound to Compute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;
};

void registerttkUncertainDataEstimator(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkUncertainDataEstimator>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
