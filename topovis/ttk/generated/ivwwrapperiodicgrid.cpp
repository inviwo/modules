#include "ivwwrapperiodicgrid.h"

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
#include "ttkPeriodicGrid.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkPeriodicGrid& filter) {
        filter.SetPeriodicity(property.get());
        return true;
    }
    BoolProperty property{"SetPeriodicity", "Periodicity in all dimensions", true};
};

struct Wrapper1 {
    bool set(ttkPeriodicGrid& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper2 {
    bool set(ttkPeriodicGrid& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkPeriodicGrid& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkPeriodicGrid> {
    static constexpr std::string_view identifier = "ttkPeriodicGrid";
    static constexpr std::string_view displayName = "TTK PeriodicGrid";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkImageData", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Output Options", {"SetPeriodicity"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;
};

void registerttkPeriodicGrid(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkPeriodicGrid>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
