#include "ivw_ttktopologicalsimplification.h"
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
#include <ttkTopologicalSimplification.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkTopologicalSimplification& filter) {
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
                              R"(Select the scalar field to process.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Domain";
};

struct Wrapper1 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputOffsetScalarField", "Force Input Offset Field",
                          R"(Check this box to force the usage of a specific input scalar field
as vertex offset (used to disambiguate flat plateaus).)"_help,
                          false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkTopologicalSimplification& filter) {
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
        CompositeProperty tmp{
            "InputOffsetField", "Input Offset Field",
            R"(Select the input offset field (to disambiguate flat plateaus).)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Domain";
};

struct Wrapper3 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetForceInputVertexScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputVertexScalarField", "Force Input Vertex ScalarField",
                          R"()"_help, false};
};

struct Wrapper4 : FieldSelection {
    bool set(ttkTopologicalSimplification& filter) {
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
        CompositeProperty tmp{"VertexIdentifierField", "Vertex Identifier Field",
                              R"(Select the vertex identifier scalar field in the sources.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Constraints";
};

struct Wrapper5 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetConsiderIdentifierAsBlackList(property.get());
        return true;
    }
    BoolProperty property{"ConsiderIdentifierAsBlackList", "Remove selected extrema",
                          R"(Check this box to remove the selected extrema (instead of
removing all non-selected extrema).)"_help,
                          false};
};

struct Wrapper6 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetUseLTS(property.get());
        return true;
    }
    BoolProperty property{"UseLTS", "Use LTS",
                          R"(Use the Localized Topological Simplification algorithm.)"_help, true};
};

struct Wrapper7 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetAddPerturbation(property.get());
        return true;
    }
    BoolProperty property{"AddPerturbation", "Numerical Perturbation",
                          R"(Numerically perturb the output (to avoid the usage of an output
offset field for flat plateau disambiguation).)"_help,
                          false};
};

struct Wrapper8 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper9 {
    bool set(ttkTopologicalSimplification& filter) {
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

struct Wrapper10 {
    bool set(ttkTopologicalSimplification& filter) {
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

struct Wrapper11 {
    bool set(ttkTopologicalSimplification& filter) {
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

struct Wrapper12 {
    bool set(ttkTopologicalSimplification& filter) {
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
struct VTKTraits<ttkTopologicalSimplification> {
    static constexpr std::string_view uri = "ttk.ttkTopologicalSimplification";
    static constexpr std::string_view className = "ttkTopologicalSimplification";
    static constexpr std::string_view identifier = "ttkTopologicalSimplification";
    static constexpr std::string_view displayName = "TTK TopologicalSimplification";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Domain", "vtkDataSet", 1, R"(Data-set to process.)"},
        InputData{
            "Constraints", "vtkDataSet", 1,
            R"(vtkPointSet that represent the constraints of the topological simplification.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{
            "Input options",
            {"Scalar Field", "ForceInputVertexScalarField", "Vertex Identifier Field",
             "ForceInputOffsetScalarField", "Input Offset Field", "ConsiderIdentifierAsBlackList"}},
        Group{"Output options", {"UseLTS", "AddPerturbation"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12>
        properties;
    static constexpr std::string_view doc =
        R"(Given an input scalar field and a list of critical points to remove,
this plugin minimally edits the scalar field such that the listed critical
points disappear. This procedure is useful to speedup subsequent topological
data analysis when outlier critical points can be easily identified. It is
also useful for data simplification.

The list of critical points to remove must be associated with a point data
scalar field that represent the vertex global identifiers in the input
geometry.

Note that this plugin will also produce an output vertex offset scalar field
that can be used for further topological data analysis tasks to disambiguate
vertices on flat plateaus. For instance, this output vertex offset field
can specified to the ContourForests, IntegralLines, or
ScalarFieldCriticalPoints plugins.

Also, this plugin can be given a specific input vertex offset.

Related publications:
"Generalized Topological Simplification of Scalar Fields on Surfaces"
Julien Tierny, Valerio Pascucci
Proc. of IEEE VIS 2012.
IEEE Transactions on Visualization and Computer Graphics, 2012.

"Localized Topological Simplification of Scalar Data"
Jonas Lukasczyk, Christoph Garth, Ross Maciejewski, Julien Tierny
Proc. of IEEE VIS 2020.
IEEE Transactions on Visualization and Computer Graphics

See also ScalarFieldCriticalPoints, IntegralLines, ContourForests,
Identifiers.

Online examples:

- https://topology-tool-kit.github.io/examples/1manifoldLearning/

- https://topology-tool-kit.github.io/examples/1manifoldLearningCircles/

- https://topology-tool-kit.github.io/examples/2manifoldLearning/

- https://topology-tool-kit.github.io/examples/BuiltInExample1/

- https://topology-tool-kit.github.io/examples/contourTreeAlignment/

- https://topology-tool-kit.github.io/examples/ctBones/

- https://topology-tool-kit.github.io/examples/dragon/

- https://topology-tool-kit.github.io/examples/harmonicSkeleton/

- https://topology-tool-kit.github.io/examples/imageProcessing/

- https://topology-tool-kit.github.io/examples/interactionSites/

- https://topology-tool-kit.github.io/examples/karhunenLoveDigits64Dimensions/

- https://topology-tool-kit.github.io/examples/morsePersistence/

- https://topology-tool-kit.github.io/examples/morseSmaleQuadrangulation/

- https://topology-tool-kit.github.io/examples/persistenceClustering0/

- https://topology-tool-kit.github.io/examples/persistenceClustering1/

- https://topology-tool-kit.github.io/examples/persistenceClustering2/

- https://topology-tool-kit.github.io/examples/persistenceClustering3/

- https://topology-tool-kit.github.io/examples/persistenceClustering4/

- https://topology-tool-kit.github.io/examples/tectonicPuzzle/

- https://topology-tool-kit.github.io/examples/tribute/

- https://topology-tool-kit.github.io/examples/uncertainStartingVortex/)";
};

void registerttkTopologicalSimplification(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkTopologicalSimplification>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
