#include "ivwwrapscalarfieldsmoother.h"

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
#include "ttkScalarFieldSmoother.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkScalarFieldSmoother& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarFieldNew", "Scalar Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkScalarFieldSmoother& filter) {
        filter.SetNumberOfIterations(property.get());
        return true;
    }
    IntProperty property{"NumberOfIterations", "Iteration Number", 1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkScalarFieldSmoother& filter) {
        filter.SetForceInputMaskScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputMaskScalarField", "Force Input Mask Scalar Field", false};
};

struct Wrapper3 : FieldSelection {
    bool set(ttkScalarFieldSmoother& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"InputMaskNew", "Mask Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper4 {
    bool set(ttkScalarFieldSmoother& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper5 {
    bool set(ttkScalarFieldSmoother& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkScalarFieldSmoother& filter) {
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
struct TTKTraits<ttkScalarFieldSmoother> {
    static constexpr std::string_view identifier = "ttkScalarFieldSmoother";
    static constexpr std::string_view displayName = "TTK ScalarFieldSmoother";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{
            "Input options",
            {"ScalarFieldNew", "NumberOfIterations", "ForceInputMaskScalarField", "InputMaskNew"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;
};

void registerttkScalarFieldSmoother(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkScalarFieldSmoother>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
