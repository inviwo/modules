#include "ivwwraparrayeditor.h"

#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/ttk/processors/ttkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include "ttkArrayEditor.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkArrayEditor& filter) {
        filter.SetEditorMode(property.get());
        return true;
    }
    OptionPropertyInt property{"EditorMode",
                               "EditorMode",
                               {{"Add Arrays from String", "Add Arrays from String", 0},
                                {"Add Arrays from Source", "Add Arrays from Source", 1},
                                {"Edit Array", "Edit Array", 2}},
                               0};
};

struct Wrapper1 {
    bool set(ttkArrayEditor& filter) {
        filter.SetTargetAssociation(property.get());
        return true;
    }
    OptionPropertyInt property{"TargetAttributeType",
                               "TargetAttributeType",
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
    StringProperty property{"DataString", "DataString", ""};
};

struct Wrapper3 {
    bool set(ttkArrayEditor& filter) {
        filter.SetReplaceExistingArrays(property.get());
        return true;
    }
    BoolProperty property{"ReplaceExistingArrays", "Replace Existing Arrays", true};
};

struct Wrapper4 : FieldSelection {
    bool set(ttkArrayEditor& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"TargetArray", "Target Array", {}, 0};

    static constexpr std::string_view inport = "Target";
};

struct Wrapper5 {
    bool set(ttkArrayEditor& filter) {
        filter.SetTargetArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"TargetArrayName", "New Array Name", ""};
};

struct Wrapper6 {
    bool set(ttkArrayEditor& filter) {
        filter.SetTargetArrayType(property.get());
        return true;
    }
    OptionPropertyInt property{"TargetArrayType",
                               "Result Array Type",
                               {{"Automatic", "Automatic", -1},
                                {"Signed Char", "Signed Char", 15},
                                {"Unsigned Char", "Unsigned Char", 4},
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
    IntVec2Property property{"TargetArrayIndexation", "Number of tuples and components",
                             ivec2{-1, -1}, std::pair{ivec2{0}, ConstraintBehavior::Ignore},
                             std::pair{ivec2{100}, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkArrayEditor& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper9 {
    bool set(ttkArrayEditor& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(ttkArrayEditor& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkArrayEditor> {
    static constexpr std::string_view identifier = "ttkArrayEditor";
    static constexpr std::string_view displayName = "TTK ArrayEditor";
    inline static std::array<InputData, 2> inports = {InputData{"Target", "vtkDataObject", -1},
                                                      InputData{"Source", "vtkDataObject", -1}};
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
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10>
        properties;
};

void registerttkArrayEditor(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkArrayEditor>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
