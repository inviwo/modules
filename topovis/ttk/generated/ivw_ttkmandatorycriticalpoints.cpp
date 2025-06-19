#include "ivw_ttkmandatorycriticalpoints.h"
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
#include <ttkMandatoryCriticalPoints.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkMandatoryCriticalPoints& filter) {
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
        CompositeProperty tmp{"LowerBoundField", "Lower Bound Field",
                              R"(Select the lower bound field to process.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 : FieldSelection {
    bool set(ttkMandatoryCriticalPoints& filter) {
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
        CompositeProperty tmp{"UpperBoundField", "Upper Bound Field",
                              R"(Select the upper bound field to process.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.SetSimplificationThreshold(property.get());
        return true;
    }
    DoubleProperty property{"SimplificationThreshold",
                            "Normalized Threshold",
                            R"(Set the simplification threshold.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.setOutputAllMinimumComponents(property.get());
        return true;
    }
    BoolProperty property{"OutputAllMinimumComponents", "Display all minima",
                          R"(Check this box to display all mandatory minima. Otherwise, specify
in the text box below which minimum to display.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.SetOutputMinimumComponentId(property.get());
        return true;
    }
    IntProperty property{"OutputMinimumComponentId",
                         "Only display minimum with Id",
                         R"(Id of the mandatory minimum to display.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.setOutputAllJoinSaddleComponents(property.get());
        return true;
    }
    BoolProperty property{"OutputAllJoinSaddleComponents", "Display all join saddles",
                          R"(Check this box to display all mandatory join saddles. Otherwise,
specify in the text box below which join saddles to display.)"_help,
                          true};
};

struct Wrapper6 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.SetOutputJoinSaddleComponentId(property.get());
        return true;
    }
    IntProperty property{"OutputJoinSaddleComponentId",
                         "Only display join saddle with Id",
                         R"(Id of the mandatory join saddle to display.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.setOutputAllSplitSaddleComponents(property.get());
        return true;
    }
    BoolProperty property{"OutputAllSplitSaddleComponents", "Display all split saddles",
                          R"(Check this box to display all mandatory split saddles. Otherwise,
specify in the text box below which split saddles to display.)"_help,
                          true};
};

struct Wrapper8 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.SetOutputSplitSaddleComponentId(property.get());
        return true;
    }
    IntProperty property{"OutputSplitSaddleComponentId",
                         "Only display split saddle with Id",
                         R"(Id of the mandatory split saddle to display.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.setOutputAllMaximumComponents(property.get());
        return true;
    }
    BoolProperty property{"OutputAllMaximumComponents", "Display all maxima",
                          R"(Check this box to display all mandatory maxima. Otherwise,
specify in the text box below which maximum to display.)"_help,
                          true};
};

struct Wrapper10 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.SetOutputMaximumComponentId(property.get());
        return true;
    }
    IntProperty property{"OutputMaxmumComponentId",
                         "Only display maximum with Id",
                         R"(Id of the mandatory maximum to display.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper11 {
    bool set(ttkMandatoryCriticalPoints& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper12 {
    bool set(ttkMandatoryCriticalPoints& filter) {
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
    bool set(ttkMandatoryCriticalPoints& filter) {
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
    bool set(ttkMandatoryCriticalPoints& filter) {
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
    bool set(ttkMandatoryCriticalPoints& filter) {
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
struct VTKTraits<ttkMandatoryCriticalPoints> {
    static constexpr std::string_view uri = "ttk.ttkMandatoryCriticalPoints";
    static constexpr std::string_view className = "ttkMandatoryCriticalPoints";
    static constexpr std::string_view identifier = "ttkMandatoryCriticalPoints";
    static constexpr std::string_view displayName = "TTK MandatoryCriticalPoints";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", 1, R"(Data-set to process.)"}};
    inline static std::array<OutputData, 6> outports = {
        OutputData{"port0", "MandatoryMinima", 0},
        OutputData{"port1", "MandatoryJoinSaddles", 1},
        OutputData{"port2", "MandatorySplitSaddles", 2},
        OutputData{"port3", "MandatoryMaxima", 3},
        OutputData{"port4", "MandatoryJoinTree", 4},
        OutputData{"port5", "MandatorySplitTree", 5}};
    inline static std::array<Group, 4> groups = {
        Group{"Simplification", {"SimplificationThreshold"}},
        Group{"Output options",
              {"OutputAllMinimumComponents", "OutputAllJoinSaddleComponents",
               "OutputAllSplitSaddleComponents", "OutputAllMaximumComponents",
               "OutputMinimumComponentId", "OutputJoinSaddleComponentId",
               "OutputSplitSaddleComponentId", "OutputMaxmumComponentId"}},
        Group{"Input options",
              {"SimplificationThreshold", "Lower Bound Field", "Upper Bound Field"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14, Wrapper15>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This filter computes the mandatory critical points of uncertain scalar
fields defined on triangulations. The input uncertain data is represented
by reliable bound fields for each vertex. In particular, the input geometry
(either 2D or 3D, either regular grids or triangulations) must be associated
with two point data scalar fields, representing the lower and upper bounds
for each vertex.

The output is a domain segmentation into the mandatory critical points.

Also, mandatory join and split trees are produced (to be visualized in a 2D
layout).

Related publication:
"Mandatory Critical Points of 2D Uncertain Scalar Fields"
David Guenther, Joseph Salmon, Julien Tierny
Proc. of EuroVis 2014.
Computer Graphics Forum, 2014.

Online Examples:

- https://topology-tool-kit.github.io/examples/uncertainStartingVortex/)ivw";
};

void registerttkMandatoryCriticalPoints(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkMandatoryCriticalPoints>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
