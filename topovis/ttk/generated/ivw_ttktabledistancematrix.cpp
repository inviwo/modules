#include "ivw_ttktabledistancematrix.h"

#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/ttk/processors/ttkgenericprocessor.h>
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
#include <ttkTableDistanceMatrix.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkTableDistanceMatrix& filter) {
        filter.SetSelectFieldsWithRegexp(property.get());
        return true;
    }
    BoolProperty property{"SelectFieldsWithRegexp", "Select Fields with a Regexp",
                          R"(Select input scalar fields matching a regular expression.)"_help,
                          false};
};

struct Wrapper1 {
    bool set(ttkTableDistanceMatrix& filter) {
        filter.SetRegexpString(property.get().c_str());
        return true;
    }
    StringProperty property{"Regexp", "Regexp",
                            R"(This regexp will be used to filter the chosen fields. Only
matching ones will be selected.)"_help,
                            ".*"};
};

struct Wrapper2 {
    bool set(ttkTableDistanceMatrix& filter) {
        filter.SetDistanceType(property.get().c_str());
        return true;
    }
    StringProperty property{"n", "p parameter",
                            R"(Value of the parameter p for the Lp distance computation
(type "inf" for the L-infinity distance).)"_help,
                            "2"};
};

struct Wrapper3 {
    bool set(ttkTableDistanceMatrix& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(ttkTableDistanceMatrix& filter) {
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
    bool set(ttkTableDistanceMatrix& filter) {
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
    bool set(ttkTableDistanceMatrix& filter) {
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
    bool set(ttkTableDistanceMatrix& filter) {
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
struct TTKTraits<ttkTableDistanceMatrix> {
    static constexpr std::string_view className = "ttkTableDistanceMatrix";
    static constexpr std::string_view identifier = "ttkTableDistanceMatrix";
    static constexpr std::string_view displayName = "TTK TableDistanceMatrix";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkTable", 1, R"(Data-set to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options", {"SelectFieldsWithRegexp", "ScalarFields", "Regexp", "n"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
    static constexpr std::string_view doc = R"(TTK Table Distance Matrix.

Online examples:

- https://topology-tool-kit.github.io/examples/persistentGenerators_householdAnalysis/

- https://topology-tool-kit.github.io/examples/persistentGenerators_periodicPicture/)";
};

void registerttkTableDistanceMatrix(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkTableDistanceMatrix>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
