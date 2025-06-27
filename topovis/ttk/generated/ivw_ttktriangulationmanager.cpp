#include "ivw_ttktriangulationmanager.h"
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
#include <ttkTriangulationManager.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkTriangulationManager& filter) {
        filter.SetPeriodicity(property.get());
        return true;
    }
    BoolProperty property{"SetPeriodicity", "Periodicity in All Dimensions",
                          R"(Set the periodicity ON or OFF.)"_help, false};
};

struct Wrapper1 {
    bool set(ttkTriangulationManager& filter) {
        filter.SetPreconditioningStrategy(property.get());
        return true;
    }
    OptionPropertyInt property{
        "SetPreconditioning",
        "Preconditioning Strategy",
        R"(Modify the preconditioning strategy for implicit and periodic triangulations:
* Default: use preconditioning under a grid size of
TTK_IMPLICIT_PRECONDITIONS_THRESHOLD^3,
* Preconditioning: force preconditioning for faster computation,
* No Preconditioning: disable preconditioning to reduce memory pressure.)"_help,
        {{"Default", "Default", 0},
         {"Preconditioning (faster)", "Preconditioning (faster)", 1},
         {"No Preconditioning (larger datasets)", "No Preconditioning (larger datasets)", 2}},
        0};
};

struct Wrapper2 {
    bool set(ttkTriangulationManager& filter) {
        filter.SetThreshold(property.get());
        return true;
    }
    IntProperty property{"Threshold",
                         "Bucket threshold",
                         R"(Bucket capacity for the octree construction.)"_help,
                         1000,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkTriangulationManager& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(ttkTriangulationManager& filter) {
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
    bool set(ttkTriangulationManager& filter) {
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
    bool set(ttkTriangulationManager& filter) {
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
    bool set(ttkTriangulationManager& filter) {
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
struct VTKTraits<ttkTriangulationManager> {
    static constexpr std::string_view uri = "ttk.ttkTriangulationManager";
    static constexpr std::string_view className = "ttkTriangulationManager";
    static constexpr std::string_view identifier = "ttkTriangulationManager";
    static constexpr std::string_view displayName = "TTK TriangulationManager";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(An input VTK data-set.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Implicit Triangulation Options", {"SetPeriodicity", "SetPreconditioning"}},
        Group{"Compact Explicit Triangulation Options", {"Threshold", "DataArrays"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This filter converts a regular grid (vtkImageData) into a
periodic regular grid (vtkImageData), in all dimensions OR compacts an
unstructured grid (vtkUnstructuredGrid, vtkPolyData) triangulation.

Alternatively, given a simplicial mesh, this filter also uses the PR star octree to divide
the mesh into different regions, and adds this clustering information as
a new scalar field to the original dataset. This clustering index scalar
field can be further used by TopoCluster data structure.

Related publications:
"The PR-star octree: A spatio-topological data structure for tetrahedral meshes."
Kenneth Weiss, Leila Floriani, Riccardo Fellegara, and Marcelo Velloso
Proc. of ACM SIGSPATIAL 2011.

"TopoCluster: A Localized Data Structure for Topology-based Visualization"
Guoxi Liu, Federico Iuricich, Riccardo Fellegara, and Leila De Floriani
IEEE Transactions on Visualization and Computer Graphics, 2021.


Online examples:

- https://topology-tool-kit.github.io/examples/compactTriangulation/

- https://topology-tool-kit.github.io/examples/clusteringKelvinHelmholtzInstabilities/)ivw";
};

void registerttkTriangulationManager(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkTriangulationManager>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
