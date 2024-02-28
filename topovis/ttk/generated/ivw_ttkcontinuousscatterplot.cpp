#include "ivw_ttkcontinuousscatterplot.h"
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
#include <ttkContinuousScatterPlot.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkContinuousScatterPlot& filter) {
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
        CompositeProperty tmp{"ScalarField1New", "Scalar Field 1",
                              R"(Scalar field to be used as first component of the
input bivariate data.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 : FieldSelection {
    bool set(ttkContinuousScatterPlot& filter) {
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
        CompositeProperty tmp{"ScalarField2New", "Scalar Field 2",
                              R"(Scalar field to be used as second component of the input
bivariate data.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(ttkContinuousScatterPlot& filter) {
        filter.SetScatterplotResolution(property.get(0), property.get(1));
        return true;
    }
    IntVec2Property property{"ScatterplotResolution",
                             "Scatterplot Resolution",
                             R"(Resolution of the scatterplot.)"_help,
                             ivec2{1920, 1080},
                             std::pair{ivec2{1, 1}, ConstraintBehavior::Ignore},
                             std::pair{ivec2{3840, 3840}, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkContinuousScatterPlot& filter) {
        filter.SetProjectImageSupport(property.get());
        return true;
    }
    BoolProperty property{"ProjectImageSupport", "Non-normalized range",
                          R"(Uncheck this box to normalize the dimensions of the continuous
scatterplot to the unit square.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(ttkContinuousScatterPlot& filter) {
        filter.SetWithDummyValue(property.get());
        return true;
    }
    BoolProperty property{
        "WithDummyValue", "With Ignored Value",
        R"(Check this box if a value should be ignored from the projection.)"_help, false};
};

struct Wrapper5 {
    bool set(ttkContinuousScatterPlot& filter) {
        filter.SetDummyValue(property.get());
        return true;
    }
    DoubleProperty property{"DummyValue",
                            "Ignored Value",
                            R"(Value to ignore.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkContinuousScatterPlot& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper7 {
    bool set(ttkContinuousScatterPlot& filter) {
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

struct Wrapper8 {
    bool set(ttkContinuousScatterPlot& filter) {
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

struct Wrapper9 {
    bool set(ttkContinuousScatterPlot& filter) {
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

struct Wrapper10 {
    bool set(ttkContinuousScatterPlot& filter) {
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
struct VTKTraits<ttkContinuousScatterPlot> {
    static constexpr std::string_view uri = "ttk.ttkContinuousScatterPlot";
    static constexpr std::string_view className = "ttkContinuousScatterPlot";
    static constexpr std::string_view identifier = "ttkContinuousScatterPlot";
    static constexpr std::string_view displayName = "TTK ContinuousScatterPlot";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", 1, R"(Data-set to process.)"}};
    inline static std::array<OutputData, 1> outports = {
        OutputData{"port0", "ProjectedTriangles", 0}};
    inline static std::array<Group, 3> groups = {
        Group{"Input options", {"ScalarField1New", "ScalarField2New"}},
        Group{"Output options",
              {"ScatterplotResolution", "ProjectImageSupport", "WithDummyValue", "DummyValue"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10>
        properties;
    static constexpr std::string_view doc =
        R"(This plugin produces a 2D vtkUnstructuredGrid with a scalar
field (named 'Density') representing the continuous scatter plot (attached
to the 2D geometry as point data). A point mask is also attached to the 2D
geometry as point data. The components of the input bivariate data must be
specified as independent scalar fields attached to the input geometry as
point data.

Related publication: 'Continuous Scatterplots', Sven Bachthaler,
Daniel Weiskopf, Proc. of IEEE VIS 2008, IEEE Transactions on Visualization and
Computer Graphics, 2008.

Online examples:

- https://topology-tool-kit.github.io/examples/builtInExample2/)";
};

void registerttkContinuousScatterPlot(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkContinuousScatterPlot>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
