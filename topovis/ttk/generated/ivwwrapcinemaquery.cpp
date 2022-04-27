#include "ivwwrapcinemaquery.h"

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
#include "ttkCinemaQuery.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkCinemaQuery& filter) {
        filter.SetSQLStatement(property.get().c_str());
        return true;
    }
    StringProperty property{"SQLStatement", "SQL Statement", "SELECT * FROM InputTable0"};
};

struct Wrapper1 {
    bool set(ttkCinemaQuery& filter) {
        filter.SetExcludeColumnsWithRegexp(property.get());
        return true;
    }
    BoolProperty property{"ExcludeColumnsWithRegexp", "Exclude columns with a Regexp", false};
};

struct Wrapper2 {
    bool set(ttkCinemaQuery& filter) {
        filter.SetRegexpString(property.get().c_str());
        return true;
    }
    StringProperty property{"Regexp", "Regexp", ".*"};
};

struct Wrapper3 {
    bool set(ttkCinemaQuery& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper4 {
    bool set(ttkCinemaQuery& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkCinemaQuery& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkCinemaQuery& filter) {
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
struct TTKTraits<ttkCinemaQuery> {
    static constexpr std::string_view identifier = "ttkCinemaQuery";
    static constexpr std::string_view displayName = "TTK CinemaQuery";
    inline static std::array<InputData, 1> inports = {InputData{"InputTable", "vtkTable", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Output Options", {"SQLStatement", "ExcludeColumnsWithRegexp", "Regexp"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;
};

void registerttkCinemaQuery(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkCinemaQuery>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
