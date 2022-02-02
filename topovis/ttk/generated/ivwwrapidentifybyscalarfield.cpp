#include "ivwwrapidentifybyscalarfield.h"

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
#include "ttkIdentifyByScalarField.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkIdentifyByScalarField& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarFieldNew", "Scalar Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkIdentifyByScalarField& filter) {
        filter.SetIncreasingOrder(property.get());
        return true;
    }
    BoolProperty property{"IncreasingOrder", "Increasing Order", false};
};

struct Wrapper2 {
    bool set(ttkIdentifyByScalarField& filter) {
        filter.SetStartByOne(property.get());
        return true;
    }
    BoolProperty property{"StartByOne", "Start By One", false};
};

struct Wrapper3 {
    bool set(ttkIdentifyByScalarField& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper4 {
    bool set(ttkIdentifyByScalarField& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkIdentifyByScalarField& filter) {
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
struct TTKTraits<ttkIdentifyByScalarField> {
    static constexpr std::string_view identifier = "ttkIdentifyByScalarField";
    static constexpr std::string_view displayName = "TTK IdentifyByScalarField";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}},
        Group{"Input options", {"ScalarFieldNew", "IncreasingOrder", "StartByOne"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;
};

void registerttkIdentifyByScalarField(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkIdentifyByScalarField>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
