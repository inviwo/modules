#include "ivw_ttkmeshsubdivision.h"
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
#include <ttkMeshSubdivision.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkMeshSubdivision& filter) {
        filter.SetIterationNumber(property.get());
        return true;
    }
    IntProperty property{"IterationNumber",
                         "Iteration Number",
                         R"(Set the number of iterations for the mesh subdivision.)"_help,
                         1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkMeshSubdivision& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper2 {
    bool set(ttkMeshSubdivision& filter) {
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
    bool set(ttkMeshSubdivision& filter) {
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
    bool set(ttkMeshSubdivision& filter) {
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
    bool set(ttkMeshSubdivision& filter) {
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
struct VTKTraits<ttkMeshSubdivision> {
    static constexpr std::string_view uri = "ttk.ttkMeshSubdivision";
    static constexpr std::string_view className = "ttkMeshSubdivision";
    static constexpr std::string_view identifier = "ttkMeshSubdivision";
    static constexpr std::string_view displayName = "TTK MeshSubdivision";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkUnstructuredGrid", -1, R"(Data-set to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options", {"IterationNumber"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;
    static constexpr std::string_view doc =
        R"(This filter subdivides an input mesh with a strategy inspired by
Discrete Morse theory. It does not modify the position of the original
vertices.

The filter inserts a new vertex at the barycenter of each d-cell C and
connects it with an edge to the new vertices inserted in the (d-1) and
(d+1)-cells that are faces or co-faces of C.

In practice, for surface meshes, this subdivision scheme corresponds to a
Catmull-Clark subdivision (without vertex displacement). It will turn a
triangle-mesh into a quad mesh, a quad mesh into a (finer) quad-mesh, a
tetrahedral mesh into a hexahedral mesh, a hexahedral mesh into a (finer)
hexadrehal mesh, etc. Generally, it will turn any 2D mesh into a quad mesh
and any 3D mesh into a hexadrehal mesh.

This filter assumes that all the cells of the input mesh are of the same
type. Also, the filter creates duplicate points, to be merged after the
fact with "Clean to Grid" under ParaView or vtkMergePoints for instance.)";
};

void registerttkMeshSubdivision(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkMeshSubdivision>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
