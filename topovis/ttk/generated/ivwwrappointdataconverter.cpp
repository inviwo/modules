#include "ivwwrappointdataconverter.h"

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
#include "ttkPointDataConverter.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkPointDataConverter& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"PointDataScalarField", "Point Data Scalar Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkPointDataConverter& filter) {
        filter.SetOutputType(property.get());
        return true;
    }
    OptionPropertyInt property{"OutputType",
                               "Output Type",
                               {{"Char", "Char", 0},
                                {"Double", "Double", 1},
                                {"Float", "Float", 2},
                                {"Int", "Int", 3},
                                {"IdType", "IdType", 4},
                                {"Short", "Short", 5},
                                {"Unsigned Short", "Unsigned Short", 6},
                                {"Unsigned Char", "Unsigned Char", 7}},
                               0};
};

struct Wrapper2 {
    bool set(ttkPointDataConverter& filter) {
        filter.SetUseNormalization(property.get());
        return true;
    }
    BoolProperty property{"NormalizeToRange", "Normalize to data type range", false};
};

struct Wrapper3 {
    bool set(ttkPointDataConverter& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper4 {
    bool set(ttkPointDataConverter& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkPointDataConverter& filter) {
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
struct TTKTraits<ttkPointDataConverter> {
    static constexpr std::string_view identifier = "ttkPointDataConverter";
    static constexpr std::string_view displayName = "TTK PointDataConverter";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}},
        Group{"Input options", {"Point Data Scalar Field"}},
        Group{"Output options", {"Output Type", "NormalizeToRange"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;
};

void registerttkPointDataConverter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkPointDataConverter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
