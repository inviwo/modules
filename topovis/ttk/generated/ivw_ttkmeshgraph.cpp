#include "ivw_ttkmeshgraph.h"
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
#include <ttkMeshGraph.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkMeshGraph& filter) {
        if (name.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, fieldAssociation.get(), name.get().c_str());
        return true;
    }
    OptionPropertyString name{"name", "Name", {}, 0};

    OptionProperty<vtkDataObject::FieldAssociations> fieldAssociation{
        "fieldAssociation",
        "Field Association",
        {{"points", "Points", vtkDataObject::FIELD_ASSOCIATION_POINTS},
         {"cells", "Cells", vtkDataObject::FIELD_ASSOCIATION_CELLS},
         {"none", "None", vtkDataObject::FIELD_ASSOCIATION_NONE},
         {"pointsThenCells", "Points then Cells",
          vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS}},
        3};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{
            "SizeArray", "Size Array",
            R"(Scalar array that contains point sizes (If you want constant scaling create a point array with the calculator filter).)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkMeshGraph& filter) {
        filter.SetSizeScale(property.get());
        return true;
    }
    DoubleProperty property{
        "SizeScale",
        "Size Scale",
        R"(A constant factor that is multiplied with the size values to conveniently scale the width of edges.)"_help,
        1.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkMeshGraph& filter) {
        filter.SetSizeAxis(property.get());
        return true;
    }
    OptionPropertyInt property{"SizeAxis",
                               "Size Axis",
                               R"(Axis on which to map size values.)"_help,
                               {{"X", "X", 0}, {"Y", "Y", 1}, {"Z", "Z", 2}},
                               1};
};

struct Wrapper3 {
    bool set(ttkMeshGraph& filter) {
        filter.SetUseQuadraticCells(property.get());
        return true;
    }
    BoolProperty property{"UseQuadraticCells", "Use Quadratic Cells",
                          R"(Mesh graph with quadratic cells or linear polygons.)"_help, true};
};

struct Wrapper4 {
    bool set(ttkMeshGraph& filter) {
        filter.SetSubdivisions(property.get());
        return true;
    }
    IntProperty property{"Subdivisions",
                         "Subdivisions",
                         R"(Number of edge subdivisions for linear polygons.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{10, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkMeshGraph& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper6 {
    bool set(ttkMeshGraph& filter) {
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

struct Wrapper7 {
    bool set(ttkMeshGraph& filter) {
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

struct Wrapper8 {
    bool set(ttkMeshGraph& filter) {
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

struct Wrapper9 {
    bool set(ttkMeshGraph& filter) {
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
struct VTKTraits<ttkMeshGraph> {
    static constexpr std::string_view uri = "ttk.ttkMeshGraph";
    static constexpr std::string_view className = "ttkMeshGraph";
    static constexpr std::string_view identifier = "ttkMeshGraph";
    static constexpr std::string_view displayName = "TTK MeshGraph";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {InputData{.identifier = "Input",
                                                                .dataType = "vtkUnstructuredGrid",
                                                                .numComp = -1,
                                                                .doc = R"(Input Graph.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input Options", {"SizeArray"}},
        Group{"Output Options", {"SizeAxis", "SizeScale", "UseQuadraticCells", "Subdivisions"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This filter generates for each one dimensional cell (edge) of a 'vtkUnstructuredGrid' a two dimensional cell by mapping a size value to the width of the input cell. The output is a 'vtkUnstructuredGrid' consisting of a set of either quadratic quads or linear polygons.

Online examples:
- https://topology-tool-kit.github.io/examples/nestedTrackingFromOverlap/)ivw";
};

void registerttkMeshGraph(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkMeshGraph>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
