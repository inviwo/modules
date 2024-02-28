#include "ivw_ttkmorphologicaloperators.h"
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
#include <ttkMorphologicalOperators.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkMorphologicalOperators& filter) {
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
        CompositeProperty tmp{"Labels", "Labels",
                              R"(The array that will be dilated or eroded.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkMorphologicalOperators& filter) {
        filter.SetMode(property.get());
        return true;
    }
    OptionPropertyInt property{
        "Mode",
        "Mode",
        R"(This option controls if the filter dilates or erodes the input array.)"_help,
        {{"Dilate", "Dilate", 0},
         {"Erode", "Erode", 1},
         {"Open", "Open", 2},
         {"Close", "Close", 3}},
        0};
};

struct Wrapper2 {
    bool set(ttkMorphologicalOperators& filter) {
        filter.SetIterations(property.get());
        return true;
    }
    IntProperty property{"Iterations",
                         "Iterations",
                         R"(Number of dilate/erode iterations.)"_help,
                         1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{10, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkMorphologicalOperators& filter) {
        filter.SetGrayscale(property.get());
        return true;
    }
    BoolProperty property{"Grayscale", "Use Grayscale Operators",
                          R"(Use the grayscale version of the morphological operators)"_help,
                          false};
};

struct Wrapper4 {
    bool set(ttkMorphologicalOperators& filter) {
        filter.SetPivotLabel(property.get().c_str());
        return true;
    }
    StringProperty property{"PivotLabel", "PivotLabel",
                            R"(The value that will be dialted or eroded.)"_help, "0"};
};

struct Wrapper5 {
    bool set(ttkMorphologicalOperators& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper6 {
    bool set(ttkMorphologicalOperators& filter) {
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
    bool set(ttkMorphologicalOperators& filter) {
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
    bool set(ttkMorphologicalOperators& filter) {
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
    bool set(ttkMorphologicalOperators& filter) {
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
struct VTKTraits<ttkMorphologicalOperators> {
    static constexpr std::string_view uri = "ttk.ttkMorphologicalOperators";
    static constexpr std::string_view className = "ttkMorphologicalOperators";
    static constexpr std::string_view identifier = "MorphologicalOperators";
    static constexpr std::string_view displayName = "TTK MorphologicalOperators";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(Input vtkDataSet.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input Options", {"Labels"}},
        Group{"Output Options", {"Mode", "Iterations", "Grayscale", "PivotLabel"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
    static constexpr std::string_view doc =
        R"(This filter either a) dilates a specified label by assigning the label of a corresponding vertex to all its neighbors, or b) erodes a specified label by assigning to a corresponding vertex the largest label among its neighbors.)";
};

void registerttkMorphologicalOperators(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkMorphologicalOperators>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
