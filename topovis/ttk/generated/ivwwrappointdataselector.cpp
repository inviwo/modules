#include "ivwwrappointdataselector.h"

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
#include "ttkPointDataSelector.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkPointDataSelector& filter) {
        filter.SetRangeId(property.get(0), property.get(1));
        return true;
    }
    IntVec2Property property{"RangeId", "Range Id", ivec2{0, 999},
                             std::pair{ivec2{0}, ConstraintBehavior::Ignore},
                             std::pair{ivec2{100}, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkPointDataSelector& filter) {
        filter.SetRegexpString(property.get().c_str());
        return true;
    }
    StringProperty property{"RegexFilter", "RegexFilter", ".*"};
};

struct Wrapper2 {
    bool set(ttkPointDataSelector& filter) {
        filter.SetRenameSelected(property.get());
        return true;
    }
    BoolProperty property{"RenameSelected", "Rename selected field", false};
};

struct Wrapper3 {
    bool set(ttkPointDataSelector& filter) {
        filter.SetSelectedFieldName(property.get().c_str());
        return true;
    }
    StringProperty property{"SelectedName", "Name for selected field", "SelectedField"};
};

struct Wrapper4 {
    bool set(ttkPointDataSelector& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper5 {
    bool set(ttkPointDataSelector& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkPointDataSelector& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkPointDataSelector& filter) {
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
struct TTKTraits<ttkPointDataSelector> {
    static constexpr std::string_view identifier = "ttkPointDataSelector";
    static constexpr std::string_view displayName = "TTK PointDataSelector";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options",
              {"ScalarFields", "RangeId", "RegexFilter", "RenameSelected", "SelectedName"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
};

void registerttkPointDataSelector(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkPointDataSelector>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
