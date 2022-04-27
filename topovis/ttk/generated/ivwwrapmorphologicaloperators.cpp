#include "ivwwrapmorphologicaloperators.h"

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
#include "ttkMorphologicalOperators.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkMorphologicalOperators& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"Labels", "Labels", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkMorphologicalOperators& filter) {
        filter.SetMode(property.get());
        return true;
    }
    OptionPropertyInt property{"Mode",
                               "Mode",
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
    IntProperty property{"Iterations", "Iterations", 1, std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{10, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkMorphologicalOperators& filter) {
        filter.SetGrayscale(property.get());
        return true;
    }
    BoolProperty property{"Grayscale", "Use Grayscale Operators", false};
};

struct Wrapper4 {
    bool set(ttkMorphologicalOperators& filter) {
        filter.SetPivotLabel(property.get().c_str());
        return true;
    }
    StringProperty property{"PivotLabel", "PivotLabel", "0"};
};

struct Wrapper5 {
    bool set(ttkMorphologicalOperators& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper6 {
    bool set(ttkMorphologicalOperators& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkMorphologicalOperators& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkMorphologicalOperators& filter) {
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
struct TTKTraits<ttkMorphologicalOperators> {
    static constexpr std::string_view identifier = "MorphologicalOperators";
    static constexpr std::string_view displayName = "TTK MorphologicalOperators";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input Options", {"Labels"}},
        Group{"Output Options", {"Mode", "Iterations", "Grayscale", "PivotLabel"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
};

void registerttkMorphologicalOperators(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkMorphologicalOperators>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
