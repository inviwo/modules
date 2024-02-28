#include "ivw_ttkdistancematrixdistortion.h"
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/vtk/processors/vtkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataObject.h>
#include <ttkDistanceMatrixDistortion.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkDistanceMatrixDistortion& filter) {
        filter.SetDoNotNormalize(property.get());
        return true;
    }
    BoolProperty property{
        "DoNotNormalize", "Do not normalize computations",
        R"(If the box is not checked, we normalize the results by dividing the sums by the maximum element of the line. Warning: if the box is checked, the values will lie between -inf and 1 (best similarity).)"_help,
        false};
};

struct Wrapper1 {
    bool set(ttkDistanceMatrixDistortion& filter) {
        filter.SetSelectFieldsWithRegexpHigh(property.get());
        return true;
    }
    BoolProperty property{"SelectFieldsWithRegexpHigh", "Select Fields with a Regexp (High matrix)",
                          R"(Select input scalar fields matching a regular expression.)"_help,
                          false};
};

struct Wrapper2 {
    bool set(ttkDistanceMatrixDistortion& filter) {
        filter.SetRegexpStringHigh(property.get().c_str());
        return true;
    }
    StringProperty property{"RegexpHigh", "RegexpHigh",
                            R"(This regexp will be used to filter the chosen fields. Only
matching ones will be selected.)"_help,
                            ".*"};
};

struct Wrapper3 {
    bool set(ttkDistanceMatrixDistortion& filter) {
        filter.SetSelectFieldsWithRegexpLow(property.get());
        return true;
    }
    BoolProperty property{"SelectFieldsWithRegexpLow", "Select Fields with a Regexp (Low matrix)",
                          R"(Select input scalar fields matching a regular expression.)"_help,
                          false};
};

struct Wrapper4 {
    bool set(ttkDistanceMatrixDistortion& filter) {
        filter.SetRegexpStringLow(property.get().c_str());
        return true;
    }
    StringProperty property{"RegexpLow", "RegexpLow",
                            R"(This regexp will be used to filter the chosen fields. Only
matching ones will be selected.)"_help,
                            ".*"};
};

struct Wrapper5 {
    bool set(ttkDistanceMatrixDistortion& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper6 {
    bool set(ttkDistanceMatrixDistortion& filter) {
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

struct Wrapper7 {
    bool set(ttkDistanceMatrixDistortion& filter) {
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

struct Wrapper8 {
    bool set(ttkDistanceMatrixDistortion& filter) {
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

struct Wrapper9 {
    bool set(ttkDistanceMatrixDistortion& filter) {
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
struct VTKTraits<ttkDistanceMatrixDistortion> {
    static constexpr std::string_view uri = "ttk.ttkDistanceMatrixDistortion";
    static constexpr std::string_view className = "ttkDistanceMatrixDistortion";
    static constexpr std::string_view identifier = "ttkDistanceMatrixDistortion";
    static constexpr std::string_view displayName = "TTK DistanceMatrixDistortion";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"HighDistanceMatrix", "vtkTable", 1, R"(High dimension distance matrix.)"},
        InputData{"LowDistanceMatrix", "vtkTable", -1, R"(Low dimension distance matrix.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options",
              {"SelectFieldsWithRegexpHigh", "ScalarFieldsHigh", "RegexpHigh",
               "SelectFieldsWithRegexpLow", "ScalarFieldsLow", "RegexpLow"}},
        Group{"Output options", {"DoNotNormalize"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
    static constexpr std::string_view doc =
        R"(This plugin, given two distance matrices representing the same points, computes the distortion between the two, according the SIM formula. It also provides, for each point, the distortion for its own distances to the other points.)";
};

void registerttkDistanceMatrixDistortion(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkDistanceMatrixDistortion>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
