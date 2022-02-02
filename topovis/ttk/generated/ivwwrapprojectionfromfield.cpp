#include "ivwwrapprojectionfromfield.h"

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
#include "ttkProjectionFromField.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkProjectionFromField& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"UComponentNew", "U Component", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 : FieldSelection {
    bool set(ttkProjectionFromField& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"VComponentNew", "V Component", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(ttkProjectionFromField& filter) {
        filter.SetUse3DCoordinatesArray(property.get());
        return true;
    }
    BoolProperty property{"Use3DCoordinatesArray", "Use 3D Coordinates Array", false};
};

struct Wrapper3 {
    bool set(ttkProjectionFromField& filter) {
        filter.SetUseTextureCoordinates(property.get());
        return true;
    }
    BoolProperty property{"UseTextureCoordinates", "Use Texture Coordinates", false};
};

struct Wrapper4 : FieldSelection {
    bool set(ttkProjectionFromField& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(2, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"3DCoordinates", "3D Coordinates", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper5 {
    bool set(ttkProjectionFromField& filter) {
        filter.SetProjectPersistenceDiagram(property.get());
        return true;
    }
    BoolProperty property{"ProjectPersistenceDiagram", "Project Persistence Diagram", false};
};

struct Wrapper6 {
    bool set(ttkProjectionFromField& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper7 {
    bool set(ttkProjectionFromField& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkProjectionFromField& filter) {
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
struct TTKTraits<ttkProjectionFromField> {
    static constexpr std::string_view identifier = "ttkProjectionFromField";
    static constexpr std::string_view displayName = "TTK ProjectionFromField";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkPointSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Input options",
              {"UComponentNew", "VComponentNew", "UseTextureCoordinates", "Use3DCoordinatesArray",
               "3DCoordinates", "ProjectPersistenceDiagram"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
};

void registerttkProjectionFromField(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkProjectionFromField>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
