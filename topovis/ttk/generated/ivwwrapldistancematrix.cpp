#include "ivwwrapldistancematrix.h"

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
#include "ttkLDistanceMatrix.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkLDistanceMatrix& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarField", "Scalar Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkLDistanceMatrix& filter) {
        filter.SetDistanceType(property.get().c_str());
        return true;
    }
    StringProperty property{"n", "p parameter", "2"};
};

struct Wrapper2 {
    bool set(ttkLDistanceMatrix& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper3 {
    bool set(ttkLDistanceMatrix& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkLDistanceMatrix& filter) {
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
struct TTKTraits<ttkLDistanceMatrix> {
    static constexpr std::string_view identifier = "ttkLDistanceMatrix";
    static constexpr std::string_view displayName = "TTK LDistanceMatrix";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkMultiBlockDataSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;
};

void registerttkLDistanceMatrix(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkLDistanceMatrix>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
