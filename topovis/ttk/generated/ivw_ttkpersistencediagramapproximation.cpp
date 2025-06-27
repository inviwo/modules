#include "ivw_ttkpersistencediagramapproximation.h"
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
#include <ttkPersistenceDiagramApproximation.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkPersistenceDiagramApproximation& filter) {
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
                              R"(Select the scalar field to process.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkPersistenceDiagramApproximation& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputOffsetScalarField", "Force Input Offset Field",
                          R"(Check this box to force the usage of a specific input scalar field
as vertex offset (used to disambiguate flat plateaus).)"_help,
                          false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkPersistenceDiagramApproximation& filter) {
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
        CompositeProperty tmp{
            "InputOffsetScalarFieldNameNew", "Input Offset Field",
            R"(Select the input offset field (used to disambiguate flat plateaus).)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkPersistenceDiagramApproximation& filter) {
        filter.SetEpsilon(property.get());
        return true;
    }
    DoubleProperty property{"Epsilon",
                            "Error",
                            R"(Tolerance on the maximal relative Bottleneck error.
It corresponds to the parameter Epsilon in the publication.
A value of 0.05 denotes a maximal relative error of 5%.)"_help,
                            0.05,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkPersistenceDiagramApproximation& filter) {
        filter.SetShowInsideDomain(property.get());
        return true;
    }
    BoolProperty property{"ShowInsideDomain", "Embed in Domain",
                          R"(Embed the persistence pairs in the domain.)"_help, false};
};

struct Wrapper5 {
    bool set(ttkPersistenceDiagramApproximation& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper6 {
    bool set(ttkPersistenceDiagramApproximation& filter) {
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
    bool set(ttkPersistenceDiagramApproximation& filter) {
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
    bool set(ttkPersistenceDiagramApproximation& filter) {
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
    bool set(ttkPersistenceDiagramApproximation& filter) {
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
struct VTKTraits<ttkPersistenceDiagramApproximation> {
    static constexpr std::string_view uri = "ttk.ttkPersistenceDiagramApproximation";
    static constexpr std::string_view className = "ttkPersistenceDiagramApproximation";
    static constexpr std::string_view identifier = "ttkPersistenceDiagramApproximation";
    static constexpr std::string_view displayName = "TTK PersistenceDiagramApproximation";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkImageData", 1, R"(Data-set to process.
Input should be a regular grid (.vti).)"}};
    inline static std::array<OutputData, 3> outports = {
        OutputData{"port0", "Persistence Diagram", 0}, OutputData{"port1", "Approximate Field", 1},
        OutputData{"port2", "Uncertainty Bounds", 2}};
    inline static std::array<Group, 3> groups = {
        Group{"Input options",
              {"ScalarFieldNew", "ForceInputOffsetScalarField", "InputOffsetScalarFieldNameNew",
               "Epsilon", "ScalarFieldNew", "ForceInputOffsetScalarField",
               "InputOffsetScalarFieldNameNew"}},
        Group{"Output options", {"ShowInsideDomain"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(TTK plugin for the computation of apprroximations of persistence diagrams.

This plugin computes an approximation of the persistence diagram of the extremum-saddle pairs
of an input scalar field.
This approach necessitates the input data to be defined on an implicit regular grid.

The approximation comes with a user-controlled error on the Bottleneck distance to the exact diagram.
The tolerance on the relative Bottleneck error is set using the parameter "Error".
An error of 0.05 corresponds to a maximal relative Bottleneck error of 5%.

This plugins produces three outputs:
1 - The approximate persistence diagram
2 - The corresponding approximation of the field (labelled "_approximated"), defined on the input domain.
3 - Representations of the uncertainty on the approximation.

Related publication
"Fast Approximation of Persistence Diagrams with Guarantees"
Jules Vidal, Julien Tierny
IEEE Symposium on Large Data Visualization and Analysis (LDAV), 2021

Online examples:)ivw";
};

void registerttkPersistenceDiagramApproximation(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkPersistenceDiagramApproximation>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
