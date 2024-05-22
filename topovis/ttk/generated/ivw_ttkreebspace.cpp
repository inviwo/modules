#include "ivw_ttkreebspace.h"
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
#include <ttkReebSpace.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkReebSpace& filter) {
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
        CompositeProperty tmp{"UComponent", "U Component",
                              R"(Select the U component of the bivariate field.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 : FieldSelection {
    bool set(ttkReebSpace& filter) {
        if (name.size() == 0) return false;
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
        3};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{"VComponent", "V Component",
                              R"(Select the V component of the bivariate field.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(ttkReebSpace& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"Withpredefinedoffset", "Force Input Offset Field",
                          R"(Check this box to force the usage of a specific input scalar field
as vertex offset (used to disambiguate flat plateaus).)"_help,
                          false};
};

struct Wrapper3 : FieldSelection {
    bool set(ttkReebSpace& filter) {
        if (name.size() == 0) return false;
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
        3};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{"UOffsetField", "U Offset Field",
                              R"(Select the scalar field to use as a vertex offset for the
u-coordinate (used to disambiguate collinear edges).)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper4 : FieldSelection {
    bool set(ttkReebSpace& filter) {
        if (name.size() == 0) return false;
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
        3};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{"VOffsetField", "V Offset Field",
                              R"(Select the scalar field to use as a vertex offset for the
v-coordinate (used to disambiguate collinear edges).)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper5 {
    bool set(ttkReebSpace& filter) {
        filter.SetSimplificationCriterion(property.get());
        return true;
    }
    OptionPropertyInt property{"SimplificationCriterion",
                               "SimplificationCriterion",
                               R"(Set the simplification criterion.)"_help,
                               {{"Domain Volume", "Domain Volume", 0},
                                {"Range Area", "Range Area", 1},
                                {"Hyper Volume", "Hyper Volume", 2}},
                               1};
};

struct Wrapper6 {
    bool set(ttkReebSpace& filter) {
        filter.SetSimplificationThreshold(property.get());
        return true;
    }
    DoubleProperty property{"SimplificationThreshold",
                            "Simplification Threshold",
                            R"(Threshold for the simplification of the Reeb space.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkReebSpace& filter) {
        filter.SetUseOctreeAcceleration(property.get());
        return true;
    }
    BoolProperty property{"OctreeAcceleration", "With Range Query Acceleration",
                          R"(Use range driven octree for range query accelerations.)"_help, true};
};

struct Wrapper8 {
    bool set(ttkReebSpace& filter) {
        filter.SetZeroSheetValue(property.get());
        return true;
    }
    BoolProperty property{"0scalar", "With field values",
                          R"(Check this box to save the input field values on the 0-sheets.)"_help,
                          true};
};

struct Wrapper9 {
    bool set(ttkReebSpace& filter) {
        filter.SetZeroSheetVertexId(property.get());
        return true;
    }
    BoolProperty property{"0vertexId", "With vertex Ids",
                          R"(Check this box to save the vertex Ids on the 0-sheets.)"_help, true};
};

struct Wrapper10 {
    bool set(ttkReebSpace& filter) {
        filter.SetZeroSheetType(property.get());
        return true;
    }
    BoolProperty property{"0type", "With sheet type",
                          R"(Check this box to save the sheet type on the 0-sheets.)"_help, true};
};

struct Wrapper11 {
    bool set(ttkReebSpace& filter) {
        filter.SetZeroSheetId(property.get());
        return true;
    }
    BoolProperty property{"0sheetId", "With sheet Ids",
                          R"(Check this box to save the sheet Ids on the 0-sheets.)"_help, true};
};

struct Wrapper12 {
    bool set(ttkReebSpace& filter) {
        filter.SetOneSheetValue(property.get());
        return true;
    }
    BoolProperty property{"1scalar", "With field values",
                          R"(Check this box to save the input field values on the 1-sheets.)"_help,
                          true};
};

struct Wrapper13 {
    bool set(ttkReebSpace& filter) {
        filter.SetOneSheetVertexId(property.get());
        return true;
    }
    BoolProperty property{"1vertexId", "With vertex Ids",
                          R"(Check this box to save the vertex Ids on the 1-sheets.)"_help, true};
};

struct Wrapper14 {
    bool set(ttkReebSpace& filter) {
        filter.SetOneSheetEdgeId(property.get());
        return true;
    }
    BoolProperty property{"1edgeId", "With edge Ids",
                          R"(Check this box to save the edge Ids on the 1-sheets.)"_help, true};
};

struct Wrapper15 {
    bool set(ttkReebSpace& filter) {
        filter.SetOneSheetType(property.get());
        return true;
    }
    BoolProperty property{"1type", "With sheet type",
                          R"(Check this box to save the sheet type on the 1-sheets.)"_help, true};
};

struct Wrapper16 {
    bool set(ttkReebSpace& filter) {
        filter.SetOneSheetId(property.get());
        return true;
    }
    BoolProperty property{"1sheetId", "With sheet Ids",
                          R"(Check this box to save the sheet Ids on the 1-sheets.)"_help, true};
};

struct Wrapper17 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheets(property.get());
        return true;
    }
    BoolProperty property{"2sheets", "With 2-sheets",
                          R"(Check this box to visualize the 2-sheets.)"_help, false};
};

struct Wrapper18 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheetValue(property.get());
        return true;
    }
    BoolProperty property{"2sheetValue", "With field values",
                          R"(Check this box to save the input field values on the 2-sheets.)"_help,
                          true};
};

struct Wrapper19 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheetParameterization(property.get());
        return true;
    }
    BoolProperty property{"2sheetParameterization", "With edge length parameterization",
                          R"(Check this box to save the edge-length parameterization
on the 2-sheets.)"_help,
                          true};
};

struct Wrapper20 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheetId(property.get());
        return true;
    }
    BoolProperty property{"2sheetId", "With sheet Ids",
                          R"(Check this box to save the sheet Ids on the 2-sheets.)"_help, true};
};

struct Wrapper21 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheetEdgeId(property.get());
        return true;
    }
    BoolProperty property{"2sheetEdgeId", "With edge Ids",
                          R"(Check this box to save the 1-sheet edgeIds on the 2-sheets.)"_help,
                          true};
};

struct Wrapper22 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheetTetId(property.get());
        return true;
    }
    BoolProperty property{"2sheetTetId", "With tet Ids",
                          R"(Check this box to save the tetIds on the 2-sheets.)"_help, true};
};

struct Wrapper23 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheetCaseId(property.get());
        return true;
    }
    BoolProperty property{"2sheetCaseId", "With case Ids",
                          R"(Check this box to save the case Ids for each 2-sheet triangle.)"_help,
                          true};
};

struct Wrapper24 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheetEdgeType(property.get());
        return true;
    }
    BoolProperty property{"2sheetEdgeType", "With edge type",
                          R"(Check this box to save the edge type of the corresponding edge in the
1-sheet.)"_help,
                          true};
};

struct Wrapper25 {
    bool set(ttkReebSpace& filter) {
        filter.SetThreeSheetVertexNumber(property.get());
        return true;
    }
    BoolProperty property{"3sheetVertexNumber", "3-sheet vertex number",
                          R"(Check this box to save the number of vertices per 3-sheet as an extra
attribute field.)"_help,
                          true};
};

struct Wrapper26 {
    bool set(ttkReebSpace& filter) {
        filter.SetThreeSheetTetNumber(property.get());
        return true;
    }
    BoolProperty property{"3sheetTetNumber", "3-sheet tet number",
                          R"(Check this box to save the number of tetrahedra per 3-sheet as an
extra attribute field.)"_help,
                          true};
};

struct Wrapper27 {
    bool set(ttkReebSpace& filter) {
        filter.SetThreeSheetExpansion(property.get());
        return true;
    }
    BoolProperty property{"3sheetExpansion", "3-sheet expansion",
                          R"(Check this box to trigger the expansion of the 3-sheets to their
maximal extent.)"_help,
                          true};
};

struct Wrapper28 {
    bool set(ttkReebSpace& filter) {
        filter.SetThreeSheetDomainVolume(property.get());
        return true;
    }
    BoolProperty property{"3sheetDomainVolume", "3-sheet domain volume",
                          R"(Check this box to save the volume in the domain of each 3-sheet as an
extra attribute field.)"_help,
                          true};
};

struct Wrapper29 {
    bool set(ttkReebSpace& filter) {
        filter.SetThreeSheetRangeArea(property.get());
        return true;
    }
    BoolProperty property{"3sheetRangeArea", "3-sheet range area",
                          R"(Check this box to save the area in the range of each 3-sheet as an
extra attribute field.)"_help,
                          true};
};

struct Wrapper30 {
    bool set(ttkReebSpace& filter) {
        filter.SetThreeSheetHyperVolume(property.get());
        return true;
    }
    BoolProperty property{"3sheetHyperVolume", "3-sheet hyper volume",
                          R"(Check this box to save the hyper-volume of each 3-sheet as an
extra attribute field.)"_help,
                          true};
};

struct Wrapper31 {
    bool set(ttkReebSpace& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper32 {
    bool set(ttkReebSpace& filter) {
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

struct Wrapper33 {
    bool set(ttkReebSpace& filter) {
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

struct Wrapper34 {
    bool set(ttkReebSpace& filter) {
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

struct Wrapper35 {
    bool set(ttkReebSpace& filter) {
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
struct VTKTraits<ttkReebSpace> {
    static constexpr std::string_view uri = "ttk.ttkReebSpace";
    static constexpr std::string_view className = "ttkReebSpace";
    static constexpr std::string_view identifier = "ttkReebSpace";
    static constexpr std::string_view displayName = "TTK ReebSpace";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", 1, R"(Data-set to process.)"}};
    inline static std::array<OutputData, 4> outports = {
        OutputData{"port0", "0-sheets", 0}, OutputData{"port1", "1-sheets", 1},
        OutputData{"port2", "2-sheets", 2}, OutputData{"port3", "3-sheets", 3}};
    inline static std::array<Group, 6> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options",
              {"U Component", "V Component", "With predefined offset", "U Offset Field",
               "V Offset Field", "SimplificationCriterion", "SimplificationThreshold",
               "OctreeAcceleration"}},
        Group{"Output options - 0-sheets", {"0-scalar", "0-vertexId", "0-type", "0-sheetId"}},
        Group{"Output options - 1-sheets",
              {"1-scalar", "1-vertexId", "1-edgeId", "1-type", "1-sheetId"}},
        Group{"Output options - 2-sheets",
              {"2-sheets", "2-sheetValue", "2-sheetParameterization", "2-sheetId", "2-sheetEdgeId",
               "2-sheetTetId", "2-sheetCaseId", "2-sheetEdgeType"}},
        Group{"Output options - 3-sheets",
              {"3-sheetVertexNumber", "3-sheetTetNumber", "3-sheetExpansion", "3-sheetDomainVolume",
               "3-sheetRangeArea", "3-sheetHyperVolume"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14, Wrapper15,
               Wrapper16, Wrapper17, Wrapper18, Wrapper19, Wrapper20, Wrapper21, Wrapper22,
               Wrapper23, Wrapper24, Wrapper25, Wrapper26, Wrapper27, Wrapper28, Wrapper29,
               Wrapper30, Wrapper31, Wrapper32, Wrapper33, Wrapper34, Wrapper35>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"(The Reeb space is a useful topological abstraction of bivariate scalar
fields for data segmentation purposes. Intuitively, it allows the automatic
separation of volumetric regions that project to the same areas in the
range. This class also implements a simplification heuristic for progressive
coarsening. Used in conjunction with continuous scatterplots, this class
enables continuous scattterplot peeling for instance.

The input data must be provided as two independent point data scalar fields
defined on the geometry.
WARNING: Only tetrahedral meshes are supported.

Related publication:
"Jacobi Fiber Surfaces for Bivariate Reeb Space Computation"
Julien Tierny, Hamish Carr
Proc. of IEEE VIS 2016.
IEEE Transactions on Visualization and Computer Graphics, 2016.

See also ContinuousScatterplot, JacobiSet, Fiber, FiberSurface,
ProjectionFromField, RangePolygon.)";
};

void registerttkReebSpace(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkReebSpace>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
