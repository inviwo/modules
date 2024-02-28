#include "ivw_ttkarrayeditor.h"
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
#include <ttkArrayEditor.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkArrayEditor& filter) {
        filter.SetEditorMode(property.get());
        return true;
    }
    OptionPropertyInt property{
        "EditorMode",
        "EditorMode",
        R"(Determines if data arrays are either a) added based on a CSV string; b) shallow copied from a source; or c) edited.)"_help,
        {{"Add Arrays from String", "Add Arrays from String", 0},
         {"Add Arrays from Source", "Add Arrays from Source", 1},
         {"Filter Arrays from Source", "Filter Arrays from Source", 2},
         {"Edit Array", "Edit Array", 3}},
        0};
};

struct Wrapper1 {
    bool set(ttkArrayEditor& filter) {
        filter.SetTargetAssociation(property.get());
        return true;
    }
    OptionPropertyInt property{
        "TargetAttributeType",
        "TargetAttributeType",
        R"(Determines if the new data arrays should be added to point, cell, or field data of the target. The default is to keep the original attribute type.)"_help,
        {{"Automatic", "Automatic", -1},
         {"Point Data", "Point Data", 0},
         {"Cell Data", "Cell Data", 1},
         {"Field Data", "Field Data", 2}},
        0};
};

struct Wrapper2 {
    bool set(ttkArrayEditor& filter) {
        filter.SetDataString(property.get().c_str());
        return true;
    }
    StringProperty property{
        "DataString", "DataString",
        R"(CSV string that will be used to create new 'vtkDataArrays'. Each row is converted into an array named after the first value (e.g. 'Time,0,0.23,0.5,1').)"_help,
        ""};
};

struct Wrapper3 {
    bool set(ttkArrayEditor& filter) {
        filter.SetReplaceExistingArrays(property.get());
        return true;
    }
    BoolProperty property{"ReplaceExistingArrays", "Replace Existing Arrays",
                          R"(Replace arrays with the same name if they already exist.)"_help, true};
};

struct Wrapper4 : FieldSelection {
    bool set(ttkArrayEditor& filter) {
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
        CompositeProperty tmp{"TargetArray", "Target Array",
                              R"(Array that will be modified.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Target";
};

struct Wrapper5 {
    bool set(ttkArrayEditor& filter) {
        filter.SetTargetArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{
        "TargetArrayName", "New Array Name",
        R"(New name of the resulting array (empty string will copy previous name).)"_help, ""};
};

struct Wrapper6 {
    bool set(ttkArrayEditor& filter) {
        filter.SetTargetArrayType(property.get());
        return true;
    }
    OptionPropertyInt property{
        "TargetArrayType",
        "Result Array Type",
        R"(This property determines the type of the edited array. The default is to keep the original data type.)"_help,
        {{"Automatic", "Automatic", -1},
         {"Signed Char", "Signed Char", 15},
         {"Unsigned Char", "Unsigned Char", 3},
         {"Short", "Short", 4},
         {"Unsigned Short", "Unsigned Short", 5},
         {"Int", "Int", 6},
         {"Unsigned Int", "Unsigned Int", 7},
         {"Long", "Long", 8},
         {"Unsigned Long", "Unsigned Long", 9},
         {"Float", "Float", 10},
         {"Double", "Double", 11},
         {"Id Type", "Id Type", 12}},
        0};
};

struct Wrapper7 {
    bool set(ttkArrayEditor& filter) {
        filter.SetTargetArrayIndexation(property.get(0), property.get(1));
        return true;
    }
    IntVec2Property property{
        "TargetArrayIndexation",
        "Number of tuples and components",
        R"(This property determines the number of tuples and components of the edited array. The default is to keep the original number.)"_help,
        ivec2{-1, -1},
        std::pair{ivec2{0, 0}, ConstraintBehavior::Ignore},
        std::pair{ivec2{100, 100}, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkArrayEditor& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper9 {
    bool set(ttkArrayEditor& filter) {
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
    bool set(ttkArrayEditor& filter) {
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
    bool set(ttkArrayEditor& filter) {
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
    bool set(ttkArrayEditor& filter) {
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
struct VTKTraits<ttkArrayEditor> {
    static constexpr std::string_view uri = "ttk.ttkArrayEditor";
    static constexpr std::string_view className = "ttkArrayEditor";
    static constexpr std::string_view identifier = "ttkArrayEditor";
    static constexpr std::string_view displayName = "TTK ArrayEditor";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Target", "vtkDataObject", -1, R"(vtkDataObject to add/edit arrays.)"},
        InputData{"Source", "vtkDataObject", -1,
                  R"(vtkDataObject that is used as a data source.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Output Options",
              {"EditorMode", "TargetArray", "TargetAttributeType", "TargetArrayName",
               "TargetArrayType", "TargetArrayIndexation"}},
        Group{"Source",
              {"DataString", "SourcePointDataArrays", "SourceCellDataArrays",
               "SourceFieldDataArrays", "SourceVertexDataArrays", "SourceEdgeDataArrays",
               "SourceRowDataArrays", "ReplaceExistingArrays"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12>
        properties;
    static constexpr std::string_view doc =
        R"(This filter adds data arrays to a 'vtkDataObject' (called target) based on a string or point/cell/field data of an optional second 'vtkDataObject' (called source). This filter can also be used to directly edit an array (including renaming, type conversion, and reindexing).

Online examples:

- https://topology-tool-kit.github.io/examples/cinemaIO/

- https://topology-tool-kit.github.io/examples/contourTreeAlignment/

- https://topology-tool-kit.github.io/examples/nestedTrackingFromOverlap/)";
};

void registerttkArrayEditor(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkArrayEditor>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
