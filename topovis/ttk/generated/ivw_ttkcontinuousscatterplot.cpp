#include "ivw_ttkcontinuousscatterplot.h"

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
#include "ttkContinuousScatterPlot.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkContinuousScatterPlot& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarField1New", "Scalar Field 1", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 : FieldSelection {
    bool set(ttkContinuousScatterPlot& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarField2New", "Scalar Field 2", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(ttkContinuousScatterPlot& filter) {
        filter.SetScatterplotResolution(property.get(0), property.get(1));
        return true;
    }
    IntVec2Property property{"ScatterplotResolution", "Scatterplot Resolution", ivec2{1920, 1080},
                             std::pair{ivec2{1}, ConstraintBehavior::Ignore},
                             std::pair{ivec2{3840}, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkContinuousScatterPlot& filter) {
        filter.SetProjectImageSupport(property.get());
        return true;
    }
    BoolProperty property{"ProjectImageSupport", "Non-normalized range", true};
};

struct Wrapper4 {
    bool set(ttkContinuousScatterPlot& filter) {
        filter.SetWithDummyValue(property.get());
        return true;
    }
    BoolProperty property{"WithDummyValue", "With Ignored Value", false};
};

struct Wrapper5 {
    bool set(ttkContinuousScatterPlot& filter) {
        filter.SetDummyValue(property.get());
        return true;
    }
    DoubleProperty property{"DummyValue", "Ignored Value", 0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkContinuousScatterPlot& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper7 {
    bool set(ttkContinuousScatterPlot& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkContinuousScatterPlot& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkContinuousScatterPlot& filter) {
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
struct TTKTraits<ttkContinuousScatterPlot> {
    static constexpr std::string_view identifier = "ttkContinuousScatterPlot";
    static constexpr std::string_view displayName = "TTK ContinuousScatterPlot";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
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
               Wrapper8, Wrapper9>
        properties;
};

void registerttkContinuousScatterPlot(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkContinuousScatterPlot>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
