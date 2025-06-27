#include "ivw_ttkicospheresfrompoints.h"
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
#include <ttkIcospheresFromPoints.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkIcospheresFromPoints& filter) {
        filter.SetNumberOfSubdivisions(property.get());
        return true;
    }
    IntProperty property{"NumberOfSubdivisions",
                         "Subdivisions",
                         R"(Number of subdevisions.)"_help,
                         2,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkIcospheresFromPoints& filter) {
        filter.SetRadius(property.get());
        return true;
    }
    DoubleProperty property{
        "Radius",
        "Radius",
        R"(Radius in world coordinates. If an input is provided then the radius is used as a scaling factor of the resulting bounding icosphere.)"_help,
        1.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkIcospheresFromPoints& filter) {
        filter.SetCopyPointData(property.get());
        return true;
    }
    BoolProperty property{"CopyPointData", "Copy Point Data",
                          R"(Add every point data array of the input to the output.)"_help, true};
};

struct Wrapper3 {
    bool set(ttkIcospheresFromPoints& filter) {
        filter.SetComputeNormals(property.get());
        return true;
    }
    BoolProperty property{"ComputeNormals", "Compute Normals",
                          R"(Controls if the output contains normals at the vertices.)"_help, true};
};

struct Wrapper4 {
    bool set(ttkIcospheresFromPoints& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(ttkIcospheresFromPoints& filter) {
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

struct Wrapper6 {
    bool set(ttkIcospheresFromPoints& filter) {
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

struct Wrapper7 {
    bool set(ttkIcospheresFromPoints& filter) {
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

struct Wrapper8 {
    bool set(ttkIcospheresFromPoints& filter) {
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
struct VTKTraits<ttkIcospheresFromPoints> {
    static constexpr std::string_view uri = "ttk.ttkIcospheresFromPoints";
    static constexpr std::string_view className = "ttkIcospheresFromPoints";
    static constexpr std::string_view identifier = "ttkIcospheresFromPoints";
    static constexpr std::string_view displayName = "TTK IcospheresFromPoints";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkPointSet", -1, R"(Object whose points will be used as ico sphere centers.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Output Options",
              {"NumberOfSubdivisions", "Radius", "CopyPointData", "ComputeNormals"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This filter creates for every vertex of an input vtkPointSet an IcoSphere with a specified number of subdivisions and radius.

Online examples:

- https://topology-tool-kit.github.io/examples/cosmicWeb/

- https://topology-tool-kit.github.io/examples/dragon/

- https://topology-tool-kit.github.io/examples/harmonicSkeleton/

- https://topology-tool-kit.github.io/examples/interactionSites/

- https://topology-tool-kit.github.io/examples/mergeTreeWAE/

- https://topology-tool-kit.github.io/examples/morseMolecule/

- https://topology-tool-kit.github.io/examples/persistenceDiagramWAE/

- https://topology-tool-kit.github.io/examples/tectonicPuzzle/)ivw";
};

void registerttkIcospheresFromPoints(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkIcospheresFromPoints>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
