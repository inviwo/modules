#include "ivw_ttkuncertaindataestimator.h"
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
#include <ttkUncertainDataEstimator.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkUncertainDataEstimator& filter) {
        filter.SetBoundToCompute(property.get());
        return true;
    }
    OptionPropertyInt property{"BoundtoCompute",
                               "Bound to Compute",
                               R"(Set what boundaries to compute.)"_help,
                               {{"Lower and Upper", "Lower and Upper", 0},
                                {"Lower only", "Lower only", 1},
                                {"Upper only", "Upper only", 2}},
                               0};
};

struct Wrapper1 {
    bool set(ttkUncertainDataEstimator& filter) {
        filter.SetBinCount(property.get());
        return true;
    }
    IntProperty property{"BinCount",
                         "Bin count",
                         R"(Thread number.)"_help,
                         10,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{1000, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkUncertainDataEstimator& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper3 {
    bool set(ttkUncertainDataEstimator& filter) {
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
    bool set(ttkUncertainDataEstimator& filter) {
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
    bool set(ttkUncertainDataEstimator& filter) {
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
    bool set(ttkUncertainDataEstimator& filter) {
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
struct VTKTraits<ttkUncertainDataEstimator> {
    static constexpr std::string_view uri = "ttk.ttkUncertainDataEstimator";
    static constexpr std::string_view className = "ttkUncertainDataEstimator";
    static constexpr std::string_view identifier = "ttkUncertainDataEstimator";
    static constexpr std::string_view displayName = "TTK UncertainDataEstimator";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", 1, R"(Data-set to process.)"}};
    inline static std::array<OutputData, 3> outports = {OutputData{"port0", "Bound Fields", 0},
                                                        OutputData{"port1", "Probabilities", 1},
                                                        OutputData{"port2", "MeanField", 2}};
    inline static std::array<Group, 4> groups = {
        Group{"Bound Fields", {"Bound to Compute"}},
        Group{"Probability Density Functions", {"BinCount"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options", {"Bound to Compute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;
    static constexpr std::string_view doc =
        R"(TTK plugin that takes an input ensemble data set (represented
by a list of scalar fields) and which computes various vertexwise statistics
(PDF estimation, bounds, moments, etc.).

See also MandatoryCriticalPoints.)";
};

void registerttkUncertainDataEstimator(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkUncertainDataEstimator>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
