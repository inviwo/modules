#include "ivw_ttktabledataselector.h"
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
#include <ttkTableDataSelector.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkTableDataSelector& filter) {
        filter.SetRangeId(property.get(0), property.get(1));
        return true;
    }
    IntVec2Property property{"RangeId",
                             "Range Id",
                             R"(First and last accepted column number.)"_help,
                             ivec2{0, 999},
                             std::pair{ivec2{0, 0}, ConstraintBehavior::Ignore},
                             std::pair{ivec2{100, 100}, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkTableDataSelector& filter) {
        filter.SetRegexpString(property.get().c_str());
        return true;
    }
    StringProperty property{
        "RegexFilter", "RegexFilter",
        R"(This regexp will be used to filter the chosen fields. Only matching ones will be selected.)"_help,
        ".*"};
};

struct Wrapper2 {
    bool set(ttkTableDataSelector& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper3 {
    bool set(ttkTableDataSelector& filter) {
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

struct Wrapper4 {
    bool set(ttkTableDataSelector& filter) {
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

struct Wrapper5 {
    bool set(ttkTableDataSelector& filter) {
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

struct Wrapper6 {
    bool set(ttkTableDataSelector& filter) {
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
struct VTKTraits<ttkTableDataSelector> {
    static constexpr std::string_view uri = "ttk.ttkTableDataSelector";
    static constexpr std::string_view className = "ttkTableDataSelector";
    static constexpr std::string_view identifier = "ttkTableDataSelector";
    static constexpr std::string_view displayName = "TTK TableDataSelector";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {InputData{.identifier = "Input",
                                                                .dataType = "vtkTable",
                                                                .numComp = -1,
                                                                .doc = R"(Data-set to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Input options", {"ScalarFields", "RangeId", "RegexFilter"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc = R"ivw(TTK tableDataSelector plugin documentation.)ivw";
};

void registerttkTableDataSelector(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkTableDataSelector>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
