#include "ivw_ttktopologicalsimplification.h"
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
        filter.SetMethod(property.get());
        return true;
    }
    OptionPropertyInt property{
        "Method",
        "Backend",
        R"(Choose the simplification algorithm.)"_help,
        {{"LTS (IEEE VIS 2020)", "LTS (IEEE VIS 2020)", 0},
         {"Legacy Approach (IEEE VIS 2012)", "Legacy Approach (IEEE VIS 2012)", 1},
         {"Topological Optimization (IEEE VIS 2024)", "Topological Optimization (IEEE VIS 2024)",
          2}},
        0};
};

struct Wrapper2 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputOffsetScalarField", "Force Input Offset Field",
                          R"(Check this box to force the usage of a specific input scalar field
as vertex offset (used to disambiguate flat plateaus).)"_help,
                          false};
};

struct Wrapper3 : FieldSelection {
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

struct Wrapper4 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetForceInputVertexScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputVertexScalarField", "Force Input Vertex ScalarField",
                          R"()"_help, false};
};

struct Wrapper5 : FieldSelection {
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

struct Wrapper6 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetConsiderIdentifierAsBlackList(property.get());
        return true;
    }
    BoolProperty property{"ConsiderIdentifierAsBlackList", "Remove selected extrema",
                          R"(Check this box to remove the selected extrema (instead of
removing all non-selected extrema).)"_help,
                          false};
};

struct Wrapper7 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetMethodOptimization(property.get());
        return true;
    }
    OptionPropertyInt property{
        "MethodOptimization",
        "Gradient Descent Backend",
        R"(Employed backend for gradient descent. Direct gradient descent provides superior time performance with regard to automatic differentiation with Adam.)"_help,
        {{"Direct gradient descent", "Direct gradient descent", 0},
         {"Adam (requires TORCH)", "Adam (requires TORCH)", 1}},
        0};
};

struct Wrapper8 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetPDCMethod(property.get());
        return true;
    }
    OptionPropertyInt property{"PDCMethod",
                               "Wasserstein Distance Backend",
                               R"(Backend for Wasserstein distance computation.
The Auction algorithm is computationally more expensive than
the progressive approach, but more accurate.)"_help,
                               {{"Progressive Approach", "Progressive Approach", 0},
                                {"Classical Auction", "Classical Auction", 1}},
                               1};
};

struct Wrapper9 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetUseFastPersistenceUpdate(property.get());
        return true;
    }
    BoolProperty property{
        "UseFastPersistenceUpdate", "Fast Persistence Update",
        R"(Check this box to use fast persistence update (i.e. the persistence diagram will not be completely recomputed from scratch but only the required information will be updated).)"_help,
        true};
};

struct Wrapper10 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetFastAssignmentUpdate(property.get());
        return true;
    }
    BoolProperty property{
        "FastAssignmentUpdate", "Fast Assignment Update",
        R"(Check this box to use the fast assignement update (i.e. persistence pairs which are still between consecutive iterations will maintain their assignments).)"_help,
        true};
};

struct Wrapper11 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetCoefStopCondition(property.get());
        return true;
    }
    DoubleProperty property{
        "CoefStopCondition",
        "Stopping Condition Coefficient",
        R"(Coefficient used in the stopping condition of the algorithm: if the fraction between the current loss and the original loss (between the input and simplified diagrams) is smaller that this coefficient, the algorithm stops.)"_help,
        0.01,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper12 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetEpochNumber(property.get());
        return true;
    }
    IntProperty property{
        "EpochNumber",
        "Maximum Iteration Number",
        R"(Maximum Iteration Number (if the stopping condition has not been satisfied yet).)"_help,
        1000,
        std::pair{0, ConstraintBehavior::Ignore},
        std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper13 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetFinePairManagement(property.get());
        return true;
    }
    OptionPropertyInt property{
        "FinePairManagement",
        "Cancellation Primitive",
        R"(Select the persistence pair cancellation primitive. For illustration, for pairs associated to topological handles of the sublevel sets, the primitive Fill-only will destroy a handle by filling a disc in its inside (only the death gradient is used). Cut-only will cut the handle (only the birth gradient is used). Fill and Cut will produce a compromise between the two (both birth and death gradients are used).)"_help,
        {{"Fill and Cut", "Fill and Cut", 0},
         {"Fill-only", "Fill-only", 1},
         {"Cut-only", "Cut-only", 2}},
        0};
};

struct Wrapper14 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetChooseLearningRate(property.get());
        return true;
    }
    BoolProperty property{"ChooseLearningRate", "Choose Learning Rate",
                          R"(Check this box to choose learning rate.)"_help, false};
};

struct Wrapper15 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetLearningRate(property.get());
        return true;
    }
    DoubleProperty property{"LearningRate",
                            "Learning Rate",
                            R"(Learning Rate.)"_help,
                            0.0001,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper16 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetAlpha(property.get());
        return true;
    }
    DoubleProperty property{"Alpha",
                            "Gradient Step Size",
                            R"(Choose the gradient step size.)"_help,
                            0.5,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper17 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetConstraintAveraging(property.get());
        return true;
    }
    BoolProperty property{
        "ConstraintAveraging", "Constraint Averaging",
        R"(If a given vertex is involved in both signal pairs (i.e. pairs to maintain) and non-signal pairs (i.e. pairs to remove), average the contributions of the constraints (otherwise, the vertex value will not change).)"_help,
        true};
};

struct Wrapper18 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetOptimizationWithoutMatching(property.get());
        return true;
    }
    BoolProperty property{
        "OptimizationWithoutMatching", "Online Ad-hoc simplification",
        R"(Enable on-line ad-hoc simplification (i.e., specify the non-signal pairs and the non-signal pairs will be re-evaluated at each iteration). Faster but less precise and more restrictive.)"_help,
        false};
};

struct Wrapper19 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetThresholdMethod(property.get());
        return true;
    }
    OptionPropertyInt property{"ThresholdMethod",
                               "Threshold Method",
                               R"(.)"_help,
                               {{"Persistence", "Persistence", 0},
                                {"PairType [Between]", "PairType [Between]", 1},
                                {"PairType [Select]", "PairType [Select]", 2}},
                               1};
};

struct Wrapper20 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetThreshold(property.get());
        return true;
    }
    DoubleProperty property{"Threshold",
                            "Threshold",
                            R"(Threshold value.)"_help,
                            0.01,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper21 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetLowerThreshold(property.get());
        return true;
    }
    IntProperty property{"LowerThreshold",
                         "Lower Threshold",
                         R"(Lower Threshold value.)"_help,
                         -1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper22 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetUpperThreshold(property.get());
        return true;
    }
    IntProperty property{"UpperThreshold",
                         "Upper Threshold",
                         R"(Upper Threshold value.)"_help,
                         2,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper23 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetPairTypeToDelete(property.get());
        return true;
    }
    IntProperty property{"PairTypeToDelete",
                         "Pair Type To Delete",
                         R"(Pair type to delete value.)"_help,
                         1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper24 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetPrintFrequency(property.get());
        return true;
    }
    IntProperty property{"PrintFrequency",
                         "Print Frequency",
                         R"(A print is made every PrintFrequency iterations.)"_help,
                         10,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper25 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetAddPerturbation(property.get());
        return true;
    }
    BoolProperty property{"AddPerturbation", "Numerical Perturbation",
                          R"(Numerically perturb the output (to avoid the usage of an output
offset field for flat plateau disambiguation).)"_help,
                          false};
};

struct Wrapper26 {
    bool set(ttkTopologicalSimplification& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper27 {
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

struct Wrapper28 {
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

struct Wrapper29 {
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

struct Wrapper30 {
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
    inline static std::array<Group, 4> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{
            "Input options",
            {"Scalar Field", "Method", "ForceInputVertexScalarField", "Vertex Identifier Field",
             "ForceInputOffsetScalarField", "Input Offset Field", "ConsiderIdentifierAsBlackList"}},
        Group{"Solver options",
              {"MethodOptimization", "PDCMethod", "UseFastPersistenceUpdate",
               "FastAssignmentUpdate", "CoefStopCondition", "EpochNumber", "FinePairManagement",
               "ChooseLearningRate", "LearningRate", "Alpha", "ConstraintAveraging",
               "OptimizationWithoutMatching", "PrintFrequency"}},
        Group{"Output options", {"AddPerturbation"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14, Wrapper15,
               Wrapper16, Wrapper17, Wrapper18, Wrapper19, Wrapper20, Wrapper21, Wrapper22,
               Wrapper23, Wrapper24, Wrapper25, Wrapper26, Wrapper27, Wrapper28, Wrapper29,
               Wrapper30>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(Given an input scalar field and a list of critical points to remove,
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
IEEE Transactions on Visualization and Computer Graphics.
Proc. of IEEE VIS 2012.

"Localized Topological Simplification of Scalar Data"
Jonas Lukasczyk, Christoph Garth, Ross Maciejewski, Julien Tierny
IEEE Transactions on Visualization and Computer Graphics.
Proc. of IEEE VIS 2020.

"A Practical Solver for Scalar Data Topological Simplification"
Mohamed Kissi, Mathieu Pont, Joshua A. Levine, Julien Tierny
IEEE Transactions on Visualization and Computer Graphics.
Proc. of IEEE VIS 2024.

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

- https://topology-tool-kit.github.io/examples/topologicalOptimization_darkSky/

- https://topology-tool-kit.github.io/examples/topologicalOptimization_pegasus/

- https://topology-tool-kit.github.io/examples/topologicalOptimization_torus/

- https://topology-tool-kit.github.io/examples/tribute/

- https://topology-tool-kit.github.io/examples/uncertainStartingVortex/)ivw";
};

void registerttkTopologicalSimplification(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkTopologicalSimplification>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
