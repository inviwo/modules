#include "ivw_ttkmergetree.h"
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
#include <ttkMergeTree.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkMergeTree& filter) {
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
                              R"(The scalar array that will be used to compute the tree.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkMergeTree& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputOffsetScalarField", "Force Input Offset Scalar Field",
                          R"(Check this box to force the usage of a specific input
scalar field as vertex offset (used to disambiguate flat
plateaus).)"_help,
                          false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkMergeTree& filter) {
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
        CompositeProperty tmp{"InputOffsetField", "Input Offset Field",
                              R"(Select the scalar field to use as a vertex offset
(used to disambiguate flat plateaus).)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkMergeTree& filter) {
        filter.SetTreeType(property.get());
        return true;
    }
    OptionPropertyInt property{"TreeType",
                               "Tree Type",
                               R"(0 is JT, 1 is ST)"_help,
                               {{"Join Tree", "Join Tree", 0}, {"Split Tree", "Split Tree", 1}},
                               0};
};

struct Wrapper4 {
    bool set(ttkMergeTree& filter) {
        filter.SetBackend(property.get());
        return true;
    }
    OptionPropertyInt property{"Backend",
                               "Backend",
                               R"(0 is FTM (IEEE TPDS 2019), 1 is ExTreeM (IEEE TVCG 2023))"_help,
                               {{"FTM (IEEE TPDS 2019)", "FTM (IEEE TPDS 2019)", 0},
                                {"ExTreeM (IEEE TVCG 2023)", "ExTreeM (IEEE TVCG 2023)", 1}},
                               0};
};

struct Wrapper5 {
    bool set(ttkMergeTree& filter) {
        filter.SetSuperArcSamplingLevel(property.get());
        return true;
    }
    IntProperty property{"SuperArcSamplingLevel",
                         "Arc Sampling",
                         R"(Super Arc Sampling Level.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkMergeTree& filter) {
        filter.SetWithNormalize(property.get());
        return true;
    }
    BoolProperty property{"NormalizeId", "Deterministic arc and node identifiers",
                          R"(Force the deterministic generation of
arc and node identifiers across distinct runs.)"_help,
                          true};
};

struct Wrapper7 {
    bool set(ttkMergeTree& filter) {
        filter.SetWithAdvStats(property.get());
        return true;
    }
    BoolProperty property{"AdvancedStats", "Advanced Statistics",
                          R"(Extra statistics on arcs and nodes (vertex numbers, distances
between nodes, ...))"_help,
                          true};
};

struct Wrapper8 {
    bool set(ttkMergeTree& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper9 {
    bool set(ttkMergeTree& filter) {
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
    bool set(ttkMergeTree& filter) {
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
    bool set(ttkMergeTree& filter) {
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
    bool set(ttkMergeTree& filter) {
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
struct VTKTraits<ttkMergeTree> {
    static constexpr std::string_view uri = "ttk.ttkMergeTree";
    static constexpr std::string_view className = "ttkMergeTree";
    static constexpr std::string_view identifier = "ttkMergeTree";
    static constexpr std::string_view displayName = "TTK MergeTree";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", 1, R"(Data-set to process.
TTK assumes that the input dataset is made of only one connected component.
If it's not the case, you can use the filter "Connectivity" (and select "Extract Largest Region").)"}};
    inline static std::array<OutputData, 3> outports = {OutputData{"port0", "Skeleton Nodes", 0},
                                                        OutputData{"port1", "Skeleton Arcs", 1},
                                                        OutputData{"port2", "Segmentation", 2}};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options",
              {"Scalar Field", "ForceInputOffsetScalarField", "Input Offset Field", "Backend"}},
        Group{"Output options",
              {"Tree Type", "SuperArcSamplingLevel", "NormalizeId", "AdvancedStats"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"(This plugin takes a scalar field attached as point data to a geometry
(either 2D or 3D, either regular grids or triangulations) and computes
its merge tree. Several outputs are produced to encode the nodes of
the tree (as points in 3D space), the arcs of the tree and the data
segmentation.

Related publication:
'Task-based Augmented Merge Trees with Fibonacci Heaps',
Charles Gueunet, Pierre Fortin, Julien Jomier, Julien Tierny,
Proc. of IEEE LDAV 2017.

CriticalType:
* 0 - minimum,
* 1 - 1saddle,
* 2 - 2saddle,
* 3 - maximum,
* 4 - degenerate,
* 5 - regular,

RegionType:
* 0 - (x,minimum),
* 1 - (x,maximum),
* 2 - (1-saddle,1-saddle),
* 3 - (2-saddle,2-saddle),
* 4 - (1-saddle,2-saddle).

Online examples:

- https://topology-tool-kit.github.io/examples/ctBones/

- https://topology-tool-kit.github.io/examples/interactionSites/

- https://topology-tool-kit.github.io/examples/mergeTreeClustering/

- https://topology-tool-kit.github.io/examples/mergeTreeFeatureTracking/

- https://topology-tool-kit.github.io/examples/mergeTreePGA/

- https://topology-tool-kit.github.io/examples/mergeTreeTemporalReduction/)";
};

void registerttkMergeTree(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkMergeTree>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
