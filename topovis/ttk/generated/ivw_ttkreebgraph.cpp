#include "ivw_ttkreebgraph.h"

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
#include <ttkReebGraph.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkReebGraph& filter) {
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

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkReebGraph& filter) {
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
    bool set(ttkReebGraph& filter) {
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
                              R"(Select the input offset scalar field to process.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkReebGraph& filter) {
        filter.SetSampling(property.get());
        return true;
    }
    IntProperty property{"ArcSampling",
                         "Arc Sampling",
                         R"(Set arc sampling level. The sample create "sampleLvl" chunk
of element in an arc and add a point at the barycenters of these elements.
Special vaues: -1 will insert all regular arcs and 0 only the super arcs (faster))"_help,
                         0,
                         std::pair{-1, ConstraintBehavior::Ignore},
                         std::pair{20, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkReebGraph& filter) {
        filter.SetSingleSweep(property.get());
        return true;
    }
    BoolProperty property{
        "SingleSweep", "Use single sweep",
        R"(Use the less efficient (but maybe more robust) single sweep version)"_help, false};
};

struct Wrapper5 {
    bool set(ttkReebGraph& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper6 {
    bool set(ttkReebGraph& filter) {
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
    bool set(ttkReebGraph& filter) {
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
    bool set(ttkReebGraph& filter) {
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
    bool set(ttkReebGraph& filter) {
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
struct TTKTraits<ttkReebGraph> {
    static constexpr std::string_view className = "ttkReebGraph";
    static constexpr std::string_view identifier = "ttkReebGraph";
    static constexpr std::string_view displayName = "TTK ReebGraph";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", 1, R"(Data-set to process.)"}};
    inline static std::array<OutputData, 3> outports = {OutputData{"port0", "Skeleton Nodes", 0},
                                                        OutputData{"port1", "Skeleton Arcs", 1},
                                                        OutputData{"port2", "Segmentation", 2}};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options",
              {"Scalar Field", "ForceInputOffsetScalarField", "Input Offset Field", "ArcSampling",
               "SingleSweep"}},
        Group{"Output options", {"ArcSampling"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
    static constexpr std::string_view doc =
        R"(TTK filter to compute the Reeb graph of a manifold data set.
This filter is based on a parallel algorithm.

related article:
Gueunet C., Fortin P., Jomier J., Tierny J.:
"Task-based Augmented Reeb Graphs with Dynamic ST-Trees"
EGPGV 2019

Online Examples:

- https://topology-tool-kit.github.io/examples/harmonicSkeleton/)";
};

void registerttkReebGraph(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkReebGraph>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
