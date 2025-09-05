#include "ivw_ttkscalarfieldcriticalpoints.h"
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
#include <ttkScalarFieldCriticalPoints.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkScalarFieldCriticalPoints& filter) {
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
        CompositeProperty tmp{"ScalarFieldNew", "Scalar Field",
                              R"(Select the scalar field to process)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"Withpredefinedoffset", "Force Input Offset Field",
                          R"(Check this box to force the usage of a specific input scalar field
as vertex offset (used to disambiguate flat plateaus).)"_help,
                          false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkScalarFieldCriticalPoints& filter) {
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
        CompositeProperty tmp{"OffsetNew", "Input Offset Field",
                              R"(Select the scalar field to use as a vertex offset
(used to disambiguate flat plateaus).)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetVertexBoundary(property.get());
        return true;
    }
    BoolProperty property{"Withboundarymask", "With boundary mask",
                          R"(Indicate if a critical point is on the boundary or not.)"_help, true};
};

struct Wrapper4 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetVertexIds(property.get());
        return true;
    }
    BoolProperty property{"Withvertexidentifiers", "With vertex identifiers",
                          R"(Add vertex identifiers on the output.)"_help, true};
};

struct Wrapper5 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetVertexScalars(property.get());
        return true;
    }
    BoolProperty property{"Withvertexscalars", "With vertex scalars",
                          R"(Add vertex scalars on the output.)"_help, true};
};

struct Wrapper6 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetBackEnd(property.get());
        return true;
    }
    OptionPropertyInt property{
        "BackEnd",
        "Backend",
        R"(Backend for the computation of critical points.)"_help,
        {{"Default generic backend", "Default generic backend", 0},
         {"Progressive Approach (IEEE TVCG 2020)", "Progressive Approach (IEEE TVCG 2020)", 1}},
        1};
};

struct Wrapper7 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetStartingResolutionLevel(property.get());
        return true;
    }
    IntProperty property{"StartingResolutionLevel",
                         "Start resolution level",
                         R"(Set the starting level of resolution for the progressive approach.
Set -1 for the finest level.)"_help,
                         0,
                         std::pair{-1, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetStoppingResolutionLevel(property.get());
        return true;
    }
    IntProperty property{"StoppingResolutionLevel",
                         "End resolution level",
                         R"(Set the ending level of resolution for the progressive approach.
Set -1 for the finest level.)"_help,
                         -1,
                         std::pair{-1, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetIsResumable(property.get());
        return true;
    }
    BoolProperty property{"IsResumable", "Resumable Computation",
                          R"(Allow resuming computation from a lower resolution)"_help, true};
};

struct Wrapper10 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetTimeLimit(property.get());
        return true;
    }
    DoubleProperty property{"TimeLimit",
                            "Time Limit (s)",
                            R"(Maximal time of computation for the progressive computation.
Set 0 for no time limit.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper11 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper12 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
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

struct Wrapper13 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
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

struct Wrapper14 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
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

struct Wrapper15 {
    bool set(ttkScalarFieldCriticalPoints& filter) {
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
struct VTKTraits<ttkScalarFieldCriticalPoints> {
    static constexpr std::string_view uri = "ttk.ttkScalarFieldCriticalPoints";
    static constexpr std::string_view className = "ttkScalarFieldCriticalPoints";
    static constexpr std::string_view identifier = "ttkScalarFieldCriticalPoints";
    static constexpr std::string_view displayName = "TTK ScalarFieldCriticalPoints";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {InputData{.identifier = "Input",
                                                                .dataType = "vtkDataSet",
                                                                .numComp = 1,
                                                                .doc = R"(Data-set to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input options",
              {"ScalarFieldNew", "With predefined offset", "OffsetNew", "BackEnd",
               "StartingResolutionLevel", "StoppingResolutionLevel", "IsResumable", "TimeLimit"}},
        Group{"Output options",
              {"With boundary mask", "With vertex identifiers", "With vertex scalars"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14, Wrapper15>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This plugin computes the list of critical points of the input scalar
field and classify them according to their type.

Two backends are available for the computation of critical points:

1) Default generic backend

Related publication:
"Critical points and curvature for embedded polyhedral surfaces"
Thomas Banchoff
American Mathematical Monthly, 1970.

This generic backend uses a Union Find data structure on the lower and
upper link of each vertex to compute its critical type.

2) A progressive approach

Related publication:
'A Progressive Approach to Scalar Field Topology'
Jules Vidal, Pierre Guillou, Julien Tierny
IEEE Transaction on Visualization and Computer Graphics, 2021

This approach requires the input data to be defined on a regular grid (implicit triangulation).
It processes the data using a multiresolution hierarchical
representation in a coarse-to-fine fashion.
Set both the Start and End resolution levels to -1 to execute in
non-progressive mode on the finest level, i.e. the original grid.
Even when using the full data hierarchy, this backend provides substantical performance improvements and is used by default for data defined on regular grids.

CriticalType:
* 0 - minimum,
* 1 - 1saddle,
* 2 - 2saddle,
* 3 - maximum,
* 4 - degenerate,
* 5 - regular,


Online examples:

- https://topology-tool-kit.github.io/examples/BuiltInExample1/

- https://topology-tool-kit.github.io/examples/compactTriangulation/

- https://topology-tool-kit.github.io/examples/dragon/

- https://topology-tool-kit.github.io/examples/interactionSites/

- https://topology-tool-kit.github.io/examples/uncertainStartingVortex/)ivw";
};

void registerttkScalarFieldCriticalPoints(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkScalarFieldCriticalPoints>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
