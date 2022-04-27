#include "ivwwraptriangulationmanager.h"

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
#include "ttkTriangulationManager.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkTriangulationManager& filter) {
        filter.SetPeriodicity(property.get());
        return true;
    }
    BoolProperty property{"SetPeriodicity", "Periodicity in All Dimensions", false};
};

struct Wrapper1 {
    bool set(ttkTriangulationManager& filter) {
        filter.SetPreconditioningStrategy(property.get());
        return true;
    }
    OptionPropertyInt property{
        "SetPreconditioning",
        "Preconditioning Strategy",
        {{"Default", "Default", 0},
         {"Preconditioning (faster)", "Preconditioning (faster)", 1},
         {"No Preconditioning (larger datasets)", "No Preconditioning (larger datasets)", 2}},
        0};
};

struct Wrapper2 {
    bool set(ttkTriangulationManager& filter) {
        filter.SetThreshold(property.get());
        return true;
    }
    IntProperty property{"Threshold", "Bucket threshold", 1000,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkTriangulationManager& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper4 {
    bool set(ttkTriangulationManager& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkTriangulationManager& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkTriangulationManager& filter) {
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
struct TTKTraits<ttkTriangulationManager> {
    static constexpr std::string_view identifier = "ttkTriangulationManager";
    static constexpr std::string_view displayName = "TTK TriangulationManager";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Implicit Triangulation Options", {"SetPeriodicity", "SetPreconditioning"}},
        Group{"Compact Explicit Triangulation Options", {"Threshold", "DataArrays"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;
};

void registerttkTriangulationManager(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkTriangulationManager>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
