#include "ivw_ttkmorsesmalequadrangulation.h"
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
#include <ttkMorseSmaleQuadrangulation.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkMorseSmaleQuadrangulation& filter) {
        filter.SetDualQuadrangulation(property.get());
        return true;
    }
    BoolProperty property{"DualQuadrangulation", "Dual Quadrangulation",
                          R"(Produce a dual quadrangulation (1 vertex per extremum).)"_help, false};
};

struct Wrapper1 {
    bool set(ttkMorseSmaleQuadrangulation& filter) {
        filter.SetShowResError(property.get());
        return true;
    }
    BoolProperty property{"ShowResError", "Force output despite warnings",
                          R"(Produces an output despite warnings.)"_help, false};
};

struct Wrapper2 {
    bool set(ttkMorseSmaleQuadrangulation& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper3 {
    bool set(ttkMorseSmaleQuadrangulation& filter) {
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
    bool set(ttkMorseSmaleQuadrangulation& filter) {
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
    bool set(ttkMorseSmaleQuadrangulation& filter) {
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
    bool set(ttkMorseSmaleQuadrangulation& filter) {
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
struct VTKTraits<ttkMorseSmaleQuadrangulation> {
    static constexpr std::string_view uri = "ttk.ttkMorseSmaleQuadrangulation";
    static constexpr std::string_view className = "ttkMorseSmaleQuadrangulation";
    static constexpr std::string_view identifier = "ttkMorseSmaleQuadrangulation";
    static constexpr std::string_view displayName = "TTK MorseSmaleQuadrangulation";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 3> inports = {
        InputData{"domain", "vtkDataSet", -1, R"(Input triangulated surface.)"},
        InputData{"criticalPoints", "vtkPointSet", 1,
                  R"(Critical points of the input Morse-Smale complex.)"},
        InputData{"separatrices", "vtkPolyData", 1,
                  R"(Separatrices of the input Morse-Smale complex.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute", "ShowResError"}},
        Group{"Input Options", {"DualQuadrangulation"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This plugin outputs a very raw quadrangulation from a
Morse-Smale Complex of a triangular surfacic mesh.

Online examples:

- https://topology-tool-kit.github.io/examples/morseSmaleQuadrangulation/)ivw";
};

void registerttkMorseSmaleQuadrangulation(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkMorseSmaleQuadrangulation>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
