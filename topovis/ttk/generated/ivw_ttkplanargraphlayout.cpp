#include "ivw_ttkplanargraphlayout.h"
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
#include <ttkPlanarGraphLayout.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetInputIsAMergeTree(property.get());
        return true;
    }
    BoolProperty property{
        "MergeTreePlanarLayout", "Input is a Merge Tree",
        R"(If enabled, this filter needs the nodes and arc output of the FTMTree filter as input and will display a specialized layout for merge tree.)"_help,
        false};
};

struct Wrapper1 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseSequences(property.get());
        return true;
    }
    BoolProperty property{
        "UseSequences", "Use Sequences",
        R"(Points are positioned along the x-axis based on a sequence (e.g., time indices or scalar values).)"_help,
        true};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkPlanarGraphLayout& filter) {
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
        CompositeProperty tmp{"SequenceArray", "Sequence Array", R"(Sequence Scalar Array.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseSizes(property.get());
        return true;
    }
    BoolProperty property{"UseSizes", "Use Sizes",
                          R"(Points cover space on the y-axis based on their size.)"_help, false};
};

struct Wrapper4 : FieldSelection {
    bool set(ttkPlanarGraphLayout& filter) {
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
        CompositeProperty tmp{"SizeArray", "Size Array", R"(Size Scalar Array.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper5 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseBranches(property.get());
        return true;
    }
    BoolProperty property{
        "UseBranches", "Use Branches",
        R"(Points with the same branch label are positioned on straight lines.)"_help, false};
};

struct Wrapper6 : FieldSelection {
    bool set(ttkPlanarGraphLayout& filter) {
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
        CompositeProperty tmp{"BranchArray", "Branch Array", R"(Branch Scalar Array.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper7 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseLevels(property.get());
        return true;
    }
    BoolProperty property{
        "UseLevels", "Use Levels",
        R"(The layout of points with the same level label are computed individually and afterwards nested based on the level hierarchy.)"_help,
        false};
};

struct Wrapper8 : FieldSelection {
    bool set(ttkPlanarGraphLayout& filter) {
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
        CompositeProperty tmp{"LevelArray", "Level Array", R"(Level Scalar Array.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper9 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetOutputArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"OutputArrayName", "Output Array Name",
                            R"(Name of the output layout array.)"_help, "Layout"};
};

struct Wrapper10 {
    bool set(ttkPlanarGraphLayout& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper11 {
    bool set(ttkPlanarGraphLayout& filter) {
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

struct Wrapper12 {
    bool set(ttkPlanarGraphLayout& filter) {
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

struct Wrapper13 {
    bool set(ttkPlanarGraphLayout& filter) {
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

struct Wrapper14 {
    bool set(ttkPlanarGraphLayout& filter) {
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
struct VTKTraits<ttkPlanarGraphLayout> {
    static constexpr std::string_view uri = "ttk.ttkPlanarGraphLayout";
    static constexpr std::string_view className = "ttkPlanarGraphLayout";
    static constexpr std::string_view identifier = "ttkPlanarGraphLayout";
    static constexpr std::string_view displayName = "TTK PlanarGraphLayout";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {InputData{.identifier = "Input",
                                                                .dataType = "vtkUnstructuredGrid",
                                                                .numComp = -1,
                                                                .doc = R"(Graph.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input Options",
              {"MergeTreePlanarLayout", "UseSequences", "SequenceArray", "UseSizes", "SizeArray",
               "UseBranches", "BranchArray", "UseLevels", "LevelArray"}},
        Group{"Output Options", {"OutputArrayName"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This filter computes a planar graph layout of a 'vtkUnstructuredGrid'. To improve the quality of the layout it is possible to pass additional field data to the algorithm:

1) Sequences: Points are positioned along the x-axis based on a sequence (e.g., time indices or scalar values).

2) Sizes: Points cover space on the y-axis based on their size.

3) Branches: Points with the same branch label are positioned on straight lines.

4) Levels: The layout of points with the same level label are computed individually and afterwards nested based on the level hierarchy. This makes it possible to draw nested graphs where each level is a layer of the resulting graph.

Related publication:

'Nested Tracking Graphs'.
Jonas Lukasczyk, Gunther Weber, Ross Maciejewski, Christoph Garth, and Heike Leitte.
Computer Graphics Forum (Special Issue, Proceedings Eurographics / IEEE Symposium on Visualization). Vol. 36. No. 3. 2017.


If the input is a merge tree, the filter can take the nodes output and the arc output of a FTMTree filter and will compute a planar layout specialized for merge tree.

Related publication:

'Wasserstein Distances, Geodesics and Barycenters of Merge Trees'
Mathieu Pont, Jules Vidal, Julie Delon, Julien Tierny.
Proc. of IEEE VIS 2021.
IEEE Transactions on Visualization and Computer Graphics, 2021

Online examples:

- https://topology-tool-kit.github.io/examples/contourTreeAlignment/

- https://topology-tool-kit.github.io/examples/nestedTrackingFromOverlap/)ivw";
};

void registerttkPlanarGraphLayout(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkPlanarGraphLayout>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
