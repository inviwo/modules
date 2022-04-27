#include "ivwwrapharmonicfield.h"

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
#include "ttkHarmonicField.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkHarmonicField& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarField", "Scalar Field", {}, 0};

    static constexpr std::string_view inport = "Constraints";
};

struct Wrapper1 {
    bool set(ttkHarmonicField& filter) {
        filter.SetForceConstraintIdentifiers(property.get());
        return true;
    }
    BoolProperty property{"ForceConstraintIdentifiers", "Force Constraint Identifiers", false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkHarmonicField& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{
        "ConstraintVerticesIdentifiers", "Constraint Vertices Identifiers", {}, 0};

    static constexpr std::string_view inport = "Constraints";
};

struct Wrapper3 {
    bool set(ttkHarmonicField& filter) {
        filter.SetUseCotanWeights(property.get());
        return true;
    }
    BoolProperty property{"UseCotanWeights", "Use Cotangent Weights", true};
};

struct Wrapper4 {
    bool set(ttkHarmonicField& filter) {
        filter.SetSolvingMethod(property.get());
        return true;
    }
    OptionPropertyInt property{
        "SolvingMethod",
        "Solver",
        {{"Auto", "Auto", 0}, {"Cholesky", "Cholesky", 1}, {"Iterative", "Iterative", 2}},
        0};
};

struct Wrapper5 {
    bool set(ttkHarmonicField& filter) {
        filter.SetLogAlpha(property.get());
        return true;
    }
    DoubleProperty property{"LogAlpha", "Penalty", 5.0, std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkHarmonicField& filter) {
        filter.SetOutputScalarFieldName(property.get().c_str());
        return true;
    }
    StringProperty property{"OutputScalarFieldName", "Output Field Name", "OutputHarmonicField"};
};

struct Wrapper7 {
    bool set(ttkHarmonicField& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper8 {
    bool set(ttkHarmonicField& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkHarmonicField& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(ttkHarmonicField& filter) {
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
struct TTKTraits<ttkHarmonicField> {
    static constexpr std::string_view identifier = "ttkHarmonicField";
    static constexpr std::string_view displayName = "TTK HarmonicField";
    inline static std::array<InputData, 2> inports = {InputData{"Domain", "vtkDataSet", -1},
                                                      InputData{"Constraints", "vtkPointSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input options",
              {"UseCotanWeights", "SolvingMethod", "Scalar Field", "ForceConstraintIdentifiers",
               "Constraint Vertices Identifiers", "LogAlpha"}},
        Group{"Output options", {"OutputScalarFieldName"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10>
        properties;
};

void registerttkHarmonicField(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkHarmonicField>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
