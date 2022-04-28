#include "ivw_ttktopologicalsimplificationbypersistence.h"

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
#include "ttkTopologicalSimplificationByPersistence.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"InputArray", "Input Array", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
        filter.SetPairType(property.get());
        return true;
    }
    OptionPropertyInt property{"PairType",
                               "PairType",
                               {{"Extremum-Saddle", "Extremum-Saddle", 0},
                                {"Minimum-Saddle", "Minimum-Saddle", 1},
                                {"Maximum-Saddle", "Maximum-Saddle", 2}},
                               0};
};

struct Wrapper2 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
        filter.SetPersistenceThreshold(property.get());
        return true;
    }
    DoubleProperty property{"PersistenceThreshold", "PersistenceThreshold", 0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
        filter.SetThresholdIsAbsolute(property.get());
        return true;
    }
    BoolProperty property{"ThresholdIsAbsolute", "ThresholdIsAbsolute", true};
};

struct Wrapper4 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
        filter.SetComputePerturbation(property.get());
        return true;
    }
    BoolProperty property{"NumericalPerturbation", "NumericalPerturbation", false};
};

struct Wrapper5 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper6 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
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
struct TTKTraits<ttkTopologicalSimplificationByPersistence> {
    static constexpr std::string_view identifier = "ttkTopologicalSimplificationByPersistence";
    static constexpr std::string_view displayName = "TTK TopologicalSimplificationByPersistence";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input Options", {"InputArray"}},
        Group{"Output Options",
              {"PairType", "PersistenceThreshold", "ThresholdIsAbsolute", "NumericalPerturbation"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
};

void registerttkTopologicalSimplificationByPersistence(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkTopologicalSimplificationByPersistence>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
