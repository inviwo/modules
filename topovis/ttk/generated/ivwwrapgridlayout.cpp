#include "ivwwrapgridlayout.h"

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
#include "ttkGridLayout.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkGridLayout& filter) {
        filter.SetColAxis(property.get());
        return true;
    }
    OptionPropertyInt property{
        "ColAxis", "Column Axis", {{"X", "X", 0}, {"Y", "Y", 1}, {"Z", "Z", 2}}, 0};
};

struct Wrapper1 {
    bool set(ttkGridLayout& filter) {
        filter.SetColGap(property.get());
        return true;
    }
    DoubleProperty property{"ColGap", "Column Gap %", 0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkGridLayout& filter) {
        filter.SetRowAxis(property.get());
        return true;
    }
    OptionPropertyInt property{
        "RowAxis", "Row Axis", {{"X", "X", 0}, {"Y", "Y", 1}, {"Z", "Z", 2}}, 1};
};

struct Wrapper3 {
    bool set(ttkGridLayout& filter) {
        filter.SetRowGap(property.get());
        return true;
    }
    DoubleProperty property{"RowGap", "Row Gap %", 0.0, std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkGridLayout& filter) {
        filter.SetNumberOfRows(property.get());
        return true;
    }
    IntProperty property{"NumberOfRows", "Number of Rows", 0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkGridLayout& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper6 {
    bool set(ttkGridLayout& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkGridLayout& filter) {
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
struct TTKTraits<ttkGridLayout> {
    static constexpr std::string_view identifier = "ttkGridLayout";
    static constexpr std::string_view displayName = "TTK GridLayout";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkMultiBlockDataSet", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Output Options", {"ColAxis", "ColGap", "RowAxis", "RowGap", "NumberOfRows"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
};

void registerttkGridLayout(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkGridLayout>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
