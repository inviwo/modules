#include "ivw_ttkcinemaquery.h"
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/vtk/processors/vtkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/ttk/util/ttkprocessorutils.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataObject.h>
#include <ttkCinemaQuery.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkCinemaQuery& filter) {
        filter.SetSQLStatement(property.get().c_str());
        return true;
    }
    StringProperty property{"SQLStatement", "SQL Statement", R"(SQL Statement.)"_help,
                            "SELECT * FROM InputTable0"};
};

struct Wrapper1 {
    bool set(ttkCinemaQuery& filter) {
        filter.SetExcludeColumnsWithRegexp(property.get());
        return true;
    }
    BoolProperty property{"ExcludeColumnsWithRegexp", "Exclude columns with a Regexp",
                          R"(Allow to exclude selected table columns from being used by
the query. The intended use case is when the input table has
more columns than allowed by SQLite.)"_help,
                          false};
};

struct Wrapper2 {
    bool set(ttkCinemaQuery& filter) {
        filter.SetRegexpString(property.get().c_str());
        return true;
    }
    StringProperty property{"Regexp", "Regexp", R"(This regexp will be used to filter the chosen
columns. Only matching ones will be selected.)"_help,
                            ".*"};
};

struct Wrapper3 {
    bool set(ttkCinemaQuery& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(ttkCinemaQuery& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber",
                         "Thread Number",
                         R"(The maximum number of threads.)"_help,
                         1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkCinemaQuery& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel",
                         "Debug Level",
                         R"(Debug level.)"_help,
                         3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkCinemaQuery& filter) {
        filter.SetCompactTriangulationCacheSize(property.get());
        return true;
    }
    DoubleProperty property{"CompactTriangulationCacheSize",
                            "Cache",
                            R"(Set the cache size for the compact triangulation as a
ratio with respect to the total cluster number.)"_help,
                            0.2,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkCinemaQuery& filter) {
        filter.Modified();
        return true;
    }
    ButtonProperty property{"Debug_Execute", "Execute",
                            R"(Executes the filter with the last applied parameters, which is
handy to re-start pipeline execution from a specific element
without changing parameters.)"_help};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<ttkCinemaQuery> {
    static constexpr std::string_view uri = "ttk.ttkCinemaQuery";
    static constexpr std::string_view className = "ttkCinemaQuery";
    static constexpr std::string_view identifier = "ttkCinemaQuery";
    static constexpr std::string_view displayName = "TTK CinemaQuery";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"InputTable", "vtkTable", -1, R"(Input vtkTables to run the SQL statement on.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Output Options", {"SQLStatement", "ExcludeColumnsWithRegexp", "Regexp"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This filter evaluates a SQL statement on multiple InputTables.

Online examples:

- https://topology-tool-kit.github.io/examples/cinemaIO/

- https://topology-tool-kit.github.io/examples/clusteringKelvinHelmholtzInstabilities/

- https://topology-tool-kit.github.io/examples/contourTreeAlignment/

- https://topology-tool-kit.github.io/examples/mergeTreeFeatureTracking/

- https://topology-tool-kit.github.io/examples/nestedTrackingFromOverlap/)ivw";
};

void registerttkCinemaQuery(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkCinemaQuery>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
