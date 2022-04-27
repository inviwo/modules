#include "ivwwraptexturemapfromfield.h"

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
#include "ttkTextureMapFromField.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkTextureMapFromField& filter) {
        filter.SetOnlyUComponent(property.get());
        return true;
    }
    BoolProperty property{"OnlyUComponent", "Only U Component", true};
};

struct Wrapper1 {
    bool set(ttkTextureMapFromField& filter) {
        filter.SetRepeatUTexture(property.get());
        return true;
    }
    BoolProperty property{"RepeatUTexture", "Repeat U Texture", false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkTextureMapFromField& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"UComponent", "U Component", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkTextureMapFromField& filter) {
        filter.SetOnlyVComponent(property.get());
        return true;
    }
    BoolProperty property{"OnlyVComponent", "Only V Component", false};
};

struct Wrapper4 {
    bool set(ttkTextureMapFromField& filter) {
        filter.SetRepeatVTexture(property.get());
        return true;
    }
    BoolProperty property{"RepeatVTexture", "Repeat V Texture", false};
};

struct Wrapper5 : FieldSelection {
    bool set(ttkTextureMapFromField& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"VComponent", "V Component", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper6 {
    bool set(ttkTextureMapFromField& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper7 {
    bool set(ttkTextureMapFromField& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkTextureMapFromField& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkTextureMapFromField& filter) {
        filter.SetCompactTriangulationCacheSize(property.get());
        return true;
    }
    DoubleProperty property{"CompactTriangulationCacheSize", "Cache", 0.2,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkTextureMapFromField> {
    static constexpr std::string_view identifier = "ttkTextureMapFromField";
    static constexpr std::string_view displayName = "TTK TextureMapFromField";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options",
              {"OnlyUComponent", "RepeatUTexture", "U Component", "OnlyVComponent",
               "RepeatVTexture", "V Component"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
};

void registerttkTextureMapFromField(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkTextureMapFromField>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
