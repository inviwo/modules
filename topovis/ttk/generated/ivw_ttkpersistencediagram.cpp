#include "ivw_ttkpersistencediagram.h"

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
#include <ttkPersistenceDiagram.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkPersistenceDiagram& filter) {
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
        CompositeProperty tmp{"ScalarFieldNew", "Scalar Field",
                              R"(Select the scalar field to process.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputOffsetScalarField", "Force Input Offset Field",
                          R"(Check this box to force the usage of a specific input scalar field
as vertex offset (used to disambiguate flat plateaus).)"_help,
                          false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkPersistenceDiagram& filter) {
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
            "InputOffsetScalarFieldNameNew", "Input Offset Field",
            R"(Select the input offset field (used to disambiguate flat plateaus).)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetBackEnd(property.get());
        return true;
    }
    OptionPropertyInt property{
        "BackEnd",
        "Backend",
        R"(Backend for the computation of the persistence diagram.
The progressive and approximate approaches only allow the computation of saddle-extremum pairs on regular grids.)"_help,
        {{"FTM (IEEE TPSD 2019)", "FTM (IEEE TPSD 2019)", 0},
         {"Progressive Approach (IEEE TVCG 2020)", "Progressive Approach (IEEE TVCG 2020)", 1},
         {"Discrete Morse Sandwich (IEEE TVCG 2023)", "Discrete Morse Sandwich (IEEE TVCG 2023)",
          2},
         {"Approximation Approach (IEEE LDAV 2021)", "Approximation Approach (IEEE LDAV 2021)", 3},
         {"Persistent Simplex (Zomorodian 2010)", "Persistent Simplex (Zomorodian 2010)", 4}},
        2};
};

struct Wrapper4 {
    bool set(ttkPersistenceDiagram& filter) {
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

struct Wrapper5 {
    bool set(ttkPersistenceDiagram& filter) {
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

struct Wrapper6 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetIsResumable(property.get());
        return true;
    }
    BoolProperty property{"IsResumable", "Resumable Computation",
                          R"(Allow resuming computation from a lower resolution)"_help, true};
};

struct Wrapper7 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetEpsilon(property.get());
        return true;
    }
    DoubleProperty property{"Epsilon",
                            "Error",
                            R"(Tolerance on the maximal relative Bottleneck error.
Corresponds to the parameter Epsilon in the publication.
An error of 0.05 denotes a maximal relative error of 5%.)"_help,
                            0.05,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkPersistenceDiagram& filter) {
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

struct Wrapper9 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetIgnoreBoundary(property.get());
        return true;
    }
    BoolProperty property{"IgnoreBoundary", "Ignore Boundary",
                          R"(Ignore the boundary component.)"_help, false};
};

struct Wrapper10 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetDMSDimensions(property.get());
        return true;
    }
    OptionPropertyInt property{
        "DMSDimensions",
        "Dimensions",
        R"(Should we compute all pairs with DiscreteMorseSandwich or a selection?)"_help,
        {{"All Dimensions", "All Dimensions", 0},
         {"Selected Dimensions (no infinite pairs)", "Selected Dimensions (no infinite pairs)", 1}},
        0};
};

struct Wrapper11 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetComputeMinSad(property.get());
        return true;
    }
    BoolProperty property{"ComputeMinSad", "Minimum-saddle diagram (dimension 0)",
                          R"(Compute the minimum-saddle pairs.)"_help, true};
};

struct Wrapper12 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetComputeSadSad(property.get());
        return true;
    }
    BoolProperty property{"ComputeSadSad", "Saddle-saddle diagram (dimension 1, slowest)",
                          R"(Compute the saddle-saddle pairs.)"_help, true};
};

struct Wrapper13 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetComputeSadMax(property.get());
        return true;
    }
    BoolProperty property{"ComputeSadMax", "Saddle-maximum diagram (dimension d - 1)",
                          R"(Compute the saddle-maximum pairs.)"_help, true};
};

struct Wrapper14 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetShowInsideDomain(property.get());
        return true;
    }
    BoolProperty property{"ShowInsideDomain", "Embed in Domain",
                          R"(Embed the persistence pairs in the domain.)"_help, false};
};

struct Wrapper15 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetClearDGCache(property.get());
        return true;
    }
    BoolProperty property{
        "ClearDGCache", "Clear DiscreteGradient Cache",
        R"(To clear the DiscreteGradient cache after computation (releases memory).)"_help, false};
};

struct Wrapper16 {
    bool set(ttkPersistenceDiagram& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper17 {
    bool set(ttkPersistenceDiagram& filter) {
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

struct Wrapper18 {
    bool set(ttkPersistenceDiagram& filter) {
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

struct Wrapper19 {
    bool set(ttkPersistenceDiagram& filter) {
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

struct Wrapper20 {
    bool set(ttkPersistenceDiagram& filter) {
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
struct TTKTraits<ttkPersistenceDiagram> {
    static constexpr std::string_view className = "ttkPersistenceDiagram";
    static constexpr std::string_view identifier = "ttkPersistenceDiagram";
    static constexpr std::string_view displayName = "TTK PersistenceDiagram";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", 1, R"(Data-set to process.
TTK assumes that the input dataset is made of only one connected component.
If it's not the case, you can use the filter "Connectivity" (and select "Extract Largest Region").)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input options",
              {"ScalarFieldNew", "ForceInputOffsetScalarField", "InputOffsetScalarFieldNameNew",
               "BackEnd", "StartingResolutionLevel", "StoppingResolutionLevel", "IsResumable",
               "TimeLimit", "Epsilon", "ScalarFieldNew", "ForceInputOffsetScalarField",
               "InputOffsetScalarFieldNameNew"}},
        Group{"Output options",
              {"Ignore Boundary", "DMSDimensions", "ComputeMinSad", "ComputeSadSad",
               "ComputeSadMax", "ClearDGCache", "ShowInsideDomain"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14, Wrapper15,
               Wrapper16, Wrapper17, Wrapper18, Wrapper19, Wrapper20>
        properties;
    static constexpr std::string_view doc =
        R"(TTK plugin for the computation of persistence diagrams.

This plugin computes the persistence diagram of the extremum-saddle pairs
of an input scalar field. The X-coordinate of each pair corresponds to its
birth, while its smallest and highest Y-coordinates correspond to its birth
and death respectively.

In practice, the diagram is represented by a vtkUnstructuredGrid. Each
vertex of this mesh represent a critical point of the input data. It is
associated with point data (vertexId, critical type). Each vertical edge
of this mesh represent a persistence pair. It is associated with cell data
(persistence of the pair, critical index of the extremum of the pair).
The diagonal of the diagram can be filtered out by considering its
PairIdentifier value (set at -1).

Persistence diagrams are useful and stable concise representations of the
topological features of a data-set. It is useful to fine-tune persistence
thresholds for topological simplification or for fast similarity
estimations for instance.

Five backends are available for the computation of the persistence diagram:

1) FTM

Related publication:
'Task-based Augmented Merge Trees with Fibonacci Heaps',
Charles Gueunet, Pierre Fortin, Julien Jomier, Julien Tierny,
Proc. of IEEE LDAV 2017.

2) A progressive approach

Related publication:
'A Progressive Approach to Scalar Field Topology'
Jules Vidal, Pierre Guillou, Julien Tierny
IEEE Transaction on Visualization and Computer Graphics, 2021

This approach necessitates the input data to be defined on an implicit regular grid.
It processes the data using a multiresolution hierarchical
representation in a coarse-to-fine fashion.
Set both the Start and End resolution levels to -1 to execute in
non-progressive mode on the finest level, i.e. the original grid.

3) Discrete Morse Sandwich (default)

Related publication:
'Discrete Morse Sandwich: Fast Computation of Persistence Diagrams for Scalar Data -- An Algorithm and A Benchmark'
Pierre Guillou, Jules Vidal, Julien Tierny
IEEE Transactions on Visualization and Computer Graphics, 2023.
arXiv:2206.13932, 2023.

Fast and versatile algorithm for persistence diagram computation.

4) Approximate Approach

Related publication
"Fast Approximation of Persistence Diagrams with Guarantees"
Jules Vidal, Julien Tierny
IEEE Symposium on Large Data Visualization and Analysis (LDAV), 2021.

This approach computes an approximation of the persistence diagram of the extremum-saddle pairs
of an input scalar field. It necessitates the input data to be defined on an implicit regular grid.

The approximation comes with a user-controlled error on the Bottleneck distance to the exact diagram.
The tolerance on the relative Bottleneck error is set using the parameter "Error".
An error of 0.05 corresponds to a maximal relative Bottleneck error of 5%.

See ttkPersistenceDiagramApproximation for more outputs, such as the approximated scalar field and visual cues
about the diagram approximation.


5) Persistent Simplex

This is a textbook (and very slow) algorithm, described in
"Algorithm and Theory of Computation Handbook (Second Edition)
- Special Topics and Techniques" by Atallah and Blanton on
page 97.


See also ContourForests, PersistenceCurve, ScalarFieldCriticalPoints,
TopologicalSimplification.

Online examples:

- https://topology-tool-kit.github.io/examples/1manifoldLearning/

- https://topology-tool-kit.github.io/examples/1manifoldLearningCircles/

- https://topology-tool-kit.github.io/examples/2manifoldLearning/

- https://topology-tool-kit.github.io/examples/BuiltInExample1/

- https://topology-tool-kit.github.io/examples/clusteringKelvinHelmholtzInstabilities/

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

- https://topology-tool-kit.github.io/examples/persistenceDiagramClustering/

- https://topology-tool-kit.github.io/examples/persistenceDiagramDistance/

- https://topology-tool-kit.github.io/examples/tectonicPuzzle/

- https://topology-tool-kit.github.io/examples/tribute/

- https://topology-tool-kit.github.io/examples/uncertainStartingVortex/)";
};

void registerttkPersistenceDiagram(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkPersistenceDiagram>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
