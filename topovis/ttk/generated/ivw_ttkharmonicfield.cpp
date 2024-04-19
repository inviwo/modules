#include "ivw_ttkharmonicfield.h"
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
#include <ttkHarmonicField.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkHarmonicField& filter) {
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
        CompositeProperty tmp{"ScalarField", "Scalar Field",
                              R"(Select the constraint value scalar field.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Constraints";
};

struct Wrapper1 {
    bool set(ttkHarmonicField& filter) {
        filter.SetForceConstraintIdentifiers(property.get());
        return true;
    }
    BoolProperty property{"ForceConstraintIdentifiers", "Force Constraint Identifiers",
                          R"(Use a non-default identifiers field for the constraint.)"_help, false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkHarmonicField& filter) {
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
        CompositeProperty tmp{"ConstraintVerticesIdentifiers", "Constraint Vertices Identifiers",
                              R"(Select the vertex identifier scalar field in the sources.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Constraints";
};

struct Wrapper3 {
    bool set(ttkHarmonicField& filter) {
        filter.SetUseCotanWeights(property.get());
        return true;
    }
    BoolProperty property{"UseCotanWeights", "Use Cotangent Weights",
                          R"(Use the more precise cotangent weights matrix instead of the
laplacian graph.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(ttkHarmonicField& filter) {
        filter.SetSolvingMethod(property.get());
        return true;
    }
    OptionPropertyInt property{
        "SolvingMethod",
        "Solver",
        R"(This property allows the user to select a solving
method. Cholesky simply decomposes the laplacian matrix, and
Iterative uses the Conjugate Gradients Iterative method to
solve the laplacian equation. Auto triggers a heuristic
which will try to select the best option between the two former.)"_help,
        {{"Auto", "Auto", 0}, {"Cholesky", "Cholesky", 1}, {"Iterative", "Iterative", 2}},
        0};
};

struct Wrapper5 {
    bool set(ttkHarmonicField& filter) {
        filter.SetLogAlpha(property.get());
        return true;
    }
    DoubleProperty property{"LogAlpha",
                            "Penalty",
                            R"(This property allows the user to select a penalty value for
solving the harmonic equation. A lower penalty should lead
to a better harmonic solution at the expense of computation
time.)"_help,
                            5.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkHarmonicField& filter) {
        filter.SetOutputScalarFieldName(property.get().c_str());
        return true;
    }
    StringProperty property{"OutputScalarFieldName", "Output Field Name",
                            R"(Select the name of the output scalar field.)"_help,
                            "OutputHarmonicField"};
};

struct Wrapper7 {
    bool set(ttkHarmonicField& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper8 {
    bool set(ttkHarmonicField& filter) {
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
    bool set(ttkHarmonicField& filter) {
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
    bool set(ttkHarmonicField& filter) {
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
    bool set(ttkHarmonicField& filter) {
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
struct VTKTraits<ttkHarmonicField> {
    static constexpr std::string_view uri = "ttk.ttkHarmonicField";
    static constexpr std::string_view className = "ttkHarmonicField";
    static constexpr std::string_view identifier = "ttkHarmonicField";
    static constexpr std::string_view displayName = "TTK HarmonicField";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Domain", "vtkDataSet", -1, R"(Data-set to process.)"},
        InputData{"Constraints", "vtkPointSet", 1, R"(Harmonic field constraints.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input options",
              {"UseCotanWeights", "SolvingMethod", "Scalar Field", "ForceConstraintIdentifiers",
               "Constraint Vertices Identifiers", "LogAlpha"}},
        Group{"Output options", {"OutputScalarFieldName"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"(This plugin takes a list of sources (a set of points with
their global identifiers attached to them) with a scalar
constraint on each of them and computes an harmonic field by
solving a laplacian equation.
Related publication
"Dynamic harmonic fields for surface processing"
Kai Xu, Hao Zhang, Daniel Cohen-Or, Yueshan Xiong
Computers and Graphics 2009.

Online Examples:

- https://topology-tool-kit.github.io/examples/harmonicSkeleton/)";
};

void registerttkHarmonicField(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkHarmonicField>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
