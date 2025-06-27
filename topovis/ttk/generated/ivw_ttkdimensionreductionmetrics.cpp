#include "ivw_ttkdimensionreductionmetrics.h"
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
#include <ttkDimensionReductionMetrics.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkDimensionReductionMetrics& filter) {
        filter.SetSelectInputFieldsWithRegexp(property.get());
        return true;
    }
    BoolProperty property{"SelectInputFieldsWithRegexp", "Select Input Fields with a Regexp",
                          R"(Select input scalar fields matching a regular expression.)"_help,
                          false};
};

struct Wrapper1 {
    bool set(ttkDimensionReductionMetrics& filter) {
        filter.SetInputRegexpString(property.get().c_str());
        return true;
    }
    StringProperty property{"InputRegexp", "InputRegexp",
                            R"(This regexp will be used to filter the chosen fields. Only
matching ones will be selected.)"_help,
                            ".*"};
};

struct Wrapper2 {
    bool set(ttkDimensionReductionMetrics& filter) {
        filter.SetSelectRepresentationFieldsWithRegexp(property.get());
        return true;
    }
    BoolProperty property{
        "SelectRepresentationFieldsWithRegexp", "Select Representation Fields with a Regexp",
        R"(Select representation scalar fields matching a regular expression.)"_help, false};
};

struct Wrapper3 {
    bool set(ttkDimensionReductionMetrics& filter) {
        filter.SetRepresentationRegexpString(property.get().c_str());
        return true;
    }
    StringProperty property{"RepresentationRegexp", "RepresentationRegexp",
                            R"(This regexp will be used to filter the chosen fields. Only
matching ones will be selected.)"_help,
                            ".*"};
};

struct Wrapper4 {
    bool set(ttkDimensionReductionMetrics& filter) {
        filter.SetWasserstein(property.get());
        return true;
    }
    IntProperty property{
        "Wasserstein",
        "Wasserstein",
        R"(Value of the parameter p for the p-th Wasserstein distances computation.)"_help,
        2,
        std::pair{0, ConstraintBehavior::Ignore},
        std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkDimensionReductionMetrics& filter) {
        filter.SetSampleSize(property.get());
        return true;
    }
    IntProperty property{
        "SampleSize",
        "Triplet sample size",
        R"(Number of triplets sampled for the estimation of the triplet accuracy. If this value is -1 all the N(N-1)(N-2)/6 triplets will be used.)"_help,
        -1,
        std::pair{0, ConstraintBehavior::Ignore},
        std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkDimensionReductionMetrics& filter) {
        filter.SetNeighborhoodSize(property.get());
        return true;
    }
    IntProperty property{
        "NeighborhoodSize",
        "Neighborhood size",
        R"(Size of the neighborhood in the rank-based measures (trustworthiness, continuity...).)"_help,
        10,
        std::pair{0, ConstraintBehavior::Ignore},
        std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkDimensionReductionMetrics& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper8 {
    bool set(ttkDimensionReductionMetrics& filter) {
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

struct Wrapper9 {
    bool set(ttkDimensionReductionMetrics& filter) {
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

struct Wrapper10 {
    bool set(ttkDimensionReductionMetrics& filter) {
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

struct Wrapper11 {
    bool set(ttkDimensionReductionMetrics& filter) {
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
struct VTKTraits<ttkDimensionReductionMetrics> {
    static constexpr std::string_view uri = "ttk.ttkDimensionReductionMetrics";
    static constexpr std::string_view className = "ttkDimensionReductionMetrics";
    static constexpr std::string_view identifier = "ttkDimensionReductionMetrics";
    static constexpr std::string_view displayName = "TTK DimensionReductionMetrics";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Input0", "vtkTable", -1, R"(High-dimensional input point cloud.)"},
        InputData{"Input1", "vtkTable", -1, R"(Low-dimensional representation.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input options", {}},
        Group{"Output options", {"Wasserstein", "SampleSize", "NeighborhoodSize"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc = R"ivw()ivw";
};

void registerttkDimensionReductionMetrics(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkDimensionReductionMetrics>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
