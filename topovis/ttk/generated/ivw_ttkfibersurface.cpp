#include "ivw_ttkfibersurface.h"

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
#include <ttkFiberSurface.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkFiberSurface& filter) {
        if (property.size() == 0) return false;
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
        0};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{
            "DomainUComponent", "Domain U Component",
            R"(Select the U component of the bivariate field on the input data.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input Domain";
};

struct Wrapper1 : FieldSelection {
    bool set(ttkFiberSurface& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, fieldAssociation.get(), name.get().c_str());
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
        0};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{
            "DomainVComponent", "Domain V Component",
            R"(Select the V component of the bivariate field on the input data.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input Domain";
};

struct Wrapper2 : FieldSelection {
    bool set(ttkFiberSurface& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(2, 0, 0, fieldAssociation.get(), name.get().c_str());
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
        0};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{
            "PolygonUComponent", "Polygon U Component",
            R"(Select the U component of the bivariate field for the range polygon.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Range Polygon";
};

struct Wrapper3 : FieldSelection {
    bool set(ttkFiberSurface& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(3, 0, 0, fieldAssociation.get(), name.get().c_str());
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
        0};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{
            "PolygonVComponent", "Polygon V Component",
            R"(Select the V component of the bivariate field for the range polygon.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Range Polygon";
};

struct Wrapper4 {
    bool set(ttkFiberSurface& filter) {
        filter.SetRangeCoordinates(property.get());
        return true;
    }
    BoolProperty property{
        "WithRangeCoordinates", "With Range Coordinates",
        R"(Stores the range coordinates for each vertex of the fiber surface.)"_help, true};
};

struct Wrapper5 {
    bool set(ttkFiberSurface& filter) {
        filter.SetEdgeParameterization(property.get());
        return true;
    }
    BoolProperty property{"WithEdgeParameterization", "With Edge Parameterization",
                          R"(Stores the edge parameterization
for each vertex of the fiber surface.)"_help,
                          true};
};

struct Wrapper6 {
    bool set(ttkFiberSurface& filter) {
        filter.SetEdgeIds(property.get());
        return true;
    }
    BoolProperty property{"WithEdgeIds", "With Edge Identifiers", R"(Stores the edge identifier
for each triangle of the fiber surface.)"_help,
                          true};
};

struct Wrapper7 {
    bool set(ttkFiberSurface& filter) {
        filter.SetTetIds(property.get());
        return true;
    }
    BoolProperty property{"WithTetIds", "With Tet Identifiers",
                          R"(Stores the tetrahedron identifier
for each triangle of the fiber surface.)"_help,
                          true};
};

struct Wrapper8 {
    bool set(ttkFiberSurface& filter) {
        filter.SetCaseIds(property.get());
        return true;
    }
    BoolProperty property{"WithCaseIds", "With Case Identifiers", R"(Stores the case identifier
for each triangle of the fiber surface.)"_help,
                          true};
};

struct Wrapper9 {
    bool set(ttkFiberSurface& filter) {
        filter.SetPointMerge(property.get());
        return true;
    }
    BoolProperty property{"WithPointMerge", "With Point Merging",
                          R"(Merges points that coincide.)"_help, false};
};

struct Wrapper10 {
    bool set(ttkFiberSurface& filter) {
        filter.SetPointMergeDistanceThreshold(property.get());
        return true;
    }
    DoubleProperty property{"PointMergeDistanceThreshold",
                            "Distance Threshold",
                            R"(Distance threshold to consider two points as coinciding.)"_help,
                            1e-06,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{10.0, ConstraintBehavior::Ignore}};
};

struct Wrapper11 {
    bool set(ttkFiberSurface& filter) {
        filter.SetRangeOctree(property.get());
        return true;
    }
    BoolProperty property{"WithOctree", "With Range Driven Octree",
                          R"(Pre-computes and uses a range driven octree to speed up
fiber surface extraction.)"_help,
                          true};
};

struct Wrapper12 {
    bool set(ttkFiberSurface& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper13 {
    bool set(ttkFiberSurface& filter) {
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

struct Wrapper14 {
    bool set(ttkFiberSurface& filter) {
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

struct Wrapper15 {
    bool set(ttkFiberSurface& filter) {
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

struct Wrapper16 {
    bool set(ttkFiberSurface& filter) {
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
struct TTKTraits<ttkFiberSurface> {
    static constexpr std::string_view className = "ttkFiberSurface";
    static constexpr std::string_view identifier = "ttkFiberSurface";
    static constexpr std::string_view displayName = "TTK FiberSurface";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Input Domain", "vtkDataSet", 1, R"(Dataset to process.)"},
        InputData{"Range Polygon", "vtkUnstructuredGrid", 1, R"(Range polygon.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 4> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options",
              {"Domain U Component", "Domain V Component", "Polygon U Component",
               "Polygon V Component"}},
        Group{"Pre-processing", {"WithOctree"}},
        Group{"Output options",
              {"WithPointMerge", "PointMergeDistanceThreshold", "WithRangeCoordinates",
               "WithEdgeParameterization", "WithEdgeIds", "WithTetIds", "WithCaseIds"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14, Wrapper15,
               Wrapper16>
        properties;
    static constexpr std::string_view doc =
        R"(Fiber surfaces are defined as the pre-images of curves drawn in the
range of bivariate volumetric functions, typically on top of the continuous
scatterplot. Fiber surfaces generalize the segmentation features of
isosurfaces to bivariate data.
This plugin implements an exact, parallel and fast algorithm for fiber
surface computation on (explicit or implicit) tetrahedral meshes.

The input bivariate data must be provided as two independent scalar fields
attached as point data to the input geometry. The input range polygon must
be provided as a vtkUnstructuredGrid with the actual 2D locations of the
vertices also provided as two independent scalar fields attached as point
data to the geometry. See the RangePolygon TTK plugin to create such an input
polygon from sparse user inputs.

Related publication:
"Fast and Exact Fiber Surface Extraction for Tetrahedral Meshes",
Pavol Klacansky, Julien Tierny, Hamish Carr, Zhao Geng
IEEE Transactions on Visualization and Computer Graphics, 2016.

Online examples:

- https://topology-tool-kit.github.io/examples/builtInExample2/)";
};

void registerttkFiberSurface(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkFiberSurface>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
