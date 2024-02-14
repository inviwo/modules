#include "ivw_ttkstablemanifoldpersistence.h"
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
#include <ttkStableManifoldPersistence.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkStableManifoldPersistence& filter) {
        filter.SetIsUnstable(property.get());
        return true;
    }
    BoolProperty property{"Unstablemanifold", "Unstable manifold",
                          R"(Indicate if the input manifolds should be interpreted as unstable
manifolds.)"_help,
                          false};
};

struct Wrapper1 {
    bool set(ttkStableManifoldPersistence& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper2 {
    bool set(ttkStableManifoldPersistence& filter) {
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

struct Wrapper3 {
    bool set(ttkStableManifoldPersistence& filter) {
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

struct Wrapper4 {
    bool set(ttkStableManifoldPersistence& filter) {
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

struct Wrapper5 {
    bool set(ttkStableManifoldPersistence& filter) {
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
struct VTKTraits<ttkStableManifoldPersistence> {
    static constexpr std::string_view uri = "ttk.ttkStableManifoldPersistence";
    static constexpr std::string_view className = "ttkStableManifoldPersistence";
    static constexpr std::string_view identifier = "ttkStableManifoldPersistence";
    static constexpr std::string_view displayName = "TTK StableManifoldPersistence";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 3> inports = {
        InputData{"Stable manifolds", "vtkDataSet", 1,
                  R"(vtkDataSet that represents stable manifolds (separatrices and
segments) obtained from the Morse-Smale complex module. This is typically any
of the three last outputs (`1-Separatrices`, or `2-Separatrices` or
`Segmentation`) of the Morse-Smale complex module.)"},
        InputData{"Critical points", "vtkDataSet", 1,
                  R"(vtkPolyData that represents the critical cells of the data. This
is typically the first output (`Critical Points`) of the Morse-Smale complex
module.)"},
        InputData{"Persistence diagram", "vtkDataSet", 1,
                  R"(vtkUnstructuredGrid that represents the persistence diagram of the
considered data set. This is typically the output of the Persistence Diagram
module.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options", {"Unstable manifold"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;
    static constexpr std::string_view doc =
        R"(Given an input stable manifold (1D, 2D or 3D) computed by the
Morse-Smale complex, this module attaches to it the persistence (and
the type) of the persistence pair of the critical point it is based on
(i.e. the critical point where the integral lines of the manifold
terminate). The inputs of this filter correspond to outputs of the
Morse-Smale complex and Persistence diagram modules (which need to be
pre-exist in the analysis pipeline).

- Input0: Stable manifold (1D, 2D or 3D, vtkDataSet generated by the
Morse-Smale complex module).
- Input1: Critical points (vtkPolyData generated by the Morse-Smale
complex module)
- Input2 Persistence diagram (vtkUnstructuredGrid generated by the
Persistence diagram module)
- Output Stable manifold with persistence and pair type

This filter can be used as any other ParaView filter.

See the related ParaView example state files for usage examples within a
VTK pipeline.

See also MorseSmaleComplex, PersistenceDiagram.)";
};

void registerttkStableManifoldPersistence(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkStableManifoldPersistence>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
