#include "ivw_ttkpersistencecurve.h"
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
#include <ttkPersistenceCurve.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkPersistenceCurve& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper1 {
    bool set(ttkPersistenceCurve& filter) {
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

struct Wrapper2 {
    bool set(ttkPersistenceCurve& filter) {
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

struct Wrapper3 {
    bool set(ttkPersistenceCurve& filter) {
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

struct Wrapper4 {
    bool set(ttkPersistenceCurve& filter) {
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
struct VTKTraits<ttkPersistenceCurve> {
    static constexpr std::string_view uri = "ttk.ttkPersistenceCurve";
    static constexpr std::string_view className = "ttkPersistenceCurve";
    static constexpr std::string_view identifier = "ttkPersistenceCurve";
    static constexpr std::string_view displayName = "TTK PersistenceCurve";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkUnstructuredGrid", -1,
                  R"(Input Persistence Diagram as computed by the TTKPersistenceDiagram filter.)"}};
    inline static std::array<OutputData, 4> outports = {
        OutputData{"port0", "Minimum-saddle pairs", 0},
        OutputData{"port1", "Saddle-saddle pairs", 1},
        OutputData{"port2", "Saddle-maximum pairs", 2}, OutputData{"port3", "All pairs", 3}};
    inline static std::array<Group, 1> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(TTK plugin for the computation of persistence curves.

This plugin takes a persistence diagram as input and computes the
number of pairs as a function of persistence (i.e. the number of pairs
whose persistence is higher than a threshold). The plugin produces tables.

A good strategy to visualize these curves is to use the "Line Chart View"
rendering type of ParaView, and to use the field "Persistence" as an X axis,
and only display the "Number of Pairs".

Such curves provide useful visual clues in order to fine-tune persistence
simplification thresholds.

See also ContourForests, PersistenceDiagram, ScalarFieldCriticalPoints, TopologicalSimplification.

Online examples:

- https://topology-tool-kit.github.io/examples/BuiltInExample1/

- https://topology-tool-kit.github.io/examples/dragon/

- https://topology-tool-kit.github.io/examples/interactionSites/

- https://topology-tool-kit.github.io/examples/morsePersistence/)ivw";
};

void registerttkPersistenceCurve(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkPersistenceCurve>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
