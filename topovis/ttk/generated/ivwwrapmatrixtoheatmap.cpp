#include "ivwwrapmatrixtoheatmap.h"

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
#include "ttkMatrixToHeatMap.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkMatrixToHeatMap& filter) {
        filter.SetSelectFieldsWithRegexp(property.get());
        return true;
    }
    BoolProperty property{"SelectFieldsWithRegexp", "Select Fields with a Regexp", false};
};

struct Wrapper1 {
    bool set(ttkMatrixToHeatMap& filter) {
        filter.SetRegexpString(property.get().c_str());
        return true;
    }
    StringProperty property{"Regexp", "Regexp", ".*"};
};

struct Wrapper2 {
    bool set(ttkMatrixToHeatMap& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper3 {
    bool set(ttkMatrixToHeatMap& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkMatrixToHeatMap& filter) {
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
struct TTKTraits<ttkMatrixToHeatMap> {
    static constexpr std::string_view identifier = "ttkMatrixToHeatMap";
    static constexpr std::string_view displayName = "TTK MatrixToHeatMap";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkTable", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Input options", {"SelectFieldsWithRegexp", "ScalarFields", "Regexp"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;
};

void registerttkMatrixToHeatMap(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkMatrixToHeatMap>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
