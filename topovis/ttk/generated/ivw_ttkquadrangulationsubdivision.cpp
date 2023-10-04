#include "ivw_ttkquadrangulationsubdivision.h"

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
#include <ttkQuadrangulationSubdivision.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetSubdivisionLevel(property.get());
        return true;
    }
    IntProperty property{"SubdivisionLevel",
                         "Level of subdivisions",
                         R"(Number of subdivisions of the original quadrangulation. The
higher the value, the more precise the output.)"_help,
                         1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetRelaxationIterations(property.get());
        return true;
    }
    IntProperty property{"RelaxationIterations",
                         "Number of relaxation iterations",
                         R"(Number of relaxation iterations.)"_help,
                         10,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetLockInputExtrema(property.get());
        return true;
    }
    BoolProperty property{"LockInputExtrema", "Lock extraordinary vertices",
                          R"(Lock the position of the input extraordinary vertices)"_help, false};
};

struct Wrapper3 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetLockAllInputVertices(property.get());
        return true;
    }
    BoolProperty property{"LockAllInputVertices", "Lock all coarse vertices",
                          R"(Lock the position of all input vertices)"_help, false};
};

struct Wrapper4 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetQuadStatistics(property.get());
        return true;
    }
    BoolProperty property{"QuadStatistics", "Quad Statistics",
                          R"(Compute geometrical statistics on the output quadrangulation.)"_help,
                          false};
};

struct Wrapper5 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetShowResError(property.get());
        return true;
    }
    BoolProperty property{"ShowResError", "Force output despite warnings",
                          R"(Produces an output despite warnings.)"_help, false};
};

struct Wrapper6 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetHausdorffLevel(property.get());
        return true;
    }
    DoubleProperty property{"HausdorffLevel",
                            "Hausdorff Warning Level",
                            R"(Set Hausdorff warning level. Produces a warning if the
maximum Hausdorff distance between the input triangulation
and the output quadrangulation exceeds this value.)"_help,
                            200.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper8 {
    bool set(ttkQuadrangulationSubdivision& filter) {
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
    bool set(ttkQuadrangulationSubdivision& filter) {
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
    bool set(ttkQuadrangulationSubdivision& filter) {
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
    bool set(ttkQuadrangulationSubdivision& filter) {
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
struct TTKTraits<ttkQuadrangulationSubdivision> {
    static constexpr std::string_view className = "ttkQuadrangulationSubdivision";
    static constexpr std::string_view identifier = "ttkQuadrangulationSubdivision";
    static constexpr std::string_view displayName = "TTK QuadrangulationSubdivision";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"mesh", "vtkDataSet", -1, R"(Input triangulated surface.)"},
        InputData{"quadrangles", "vtkPolyData", -1, R"(Input coarse quadrangulation.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute", "HausdorffLevel", "ShowResError"}},
        Group{"Input options",
              {"SubdivisionLevel", "RelaxationIterations", "LockInputExtrema",
               "LockAllInputVertices", "QuadStatistics"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11>
        properties;
    static constexpr std::string_view doc =
        R"(This plugin outputs a very raw quadrangulation from a
Morse-Smale Complex of a triangular surfacic mesh.

Online examples:

- https://topology-tool-kit.github.io/examples/morseSmaleQuadrangulation/)";
};

void registerttkQuadrangulationSubdivision(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkQuadrangulationSubdivision>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
