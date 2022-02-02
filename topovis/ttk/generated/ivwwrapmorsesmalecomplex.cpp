#include "ivwwrapmorsesmalecomplex.h"

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
#include "ttkMorseSmaleComplex.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkMorseSmaleComplex& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarField", "Scalar Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputOffsetScalarField", "Force Input Offset Field", false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkMorseSmaleComplex& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"OffsetField", "Offset Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeCriticalPoints(property.get());
        return true;
    }
    BoolProperty property{"ComputeCriticalPoints", "Critical Points", true};
};

struct Wrapper4 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeAscendingSeparatrices1(property.get());
        return true;
    }
    BoolProperty property{"ComputeAscendingSeparatrices1", "Ascending 1-Separatrices", true};
};

struct Wrapper5 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeDescendingSeparatrices1(property.get());
        return true;
    }
    BoolProperty property{"ComputeDescendingSeparatrices1", "Descending 1-Separatrices", true};
};

struct Wrapper6 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeSaddleConnectors(property.get());
        return true;
    }
    BoolProperty property{"ComputeSaddleConnectors", "Saddle Connectors", true};
};

struct Wrapper7 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeAscendingSeparatrices2(property.get());
        return true;
    }
    BoolProperty property{"ComputeAscendingSeparatrices2", "Ascending 2-Separatrices", false};
};

struct Wrapper8 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeDescendingSeparatrices2(property.get());
        return true;
    }
    BoolProperty property{"ComputeDescendingSeparatrices2", "Descending 2-Separatrices", false};
};

struct Wrapper9 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeAscendingSegmentation(property.get());
        return true;
    }
    BoolProperty property{"ComputeAscendingSegmentation", "Ascending Segmentation", true};
};

struct Wrapper10 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeDescendingSegmentation(property.get());
        return true;
    }
    BoolProperty property{"ComputeDescendingSegmentation", "Descending Segmentation", true};
};

struct Wrapper11 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeFinalSegmentation(property.get());
        return true;
    }
    BoolProperty property{"ComputeFinalSegmentation", "Morse-Smale Complex Segmentation", true};
};

struct Wrapper12 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetReturnSaddleConnectors(property.get());
        return true;
    }
    BoolProperty property{"ReturnSaddleConnectors", "Return Saddle Connectors", false};
};

struct Wrapper13 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetSaddleConnectorsPersistenceThreshold(property.get());
        return true;
    }
    DoubleProperty property{"SaddleConnectorsPersistenceThreshold",
                            "Saddle Connectors Persistence Threshold", 0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100000.0, ConstraintBehavior::Ignore}};
};

struct Wrapper14 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper15 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper16 {
    bool set(ttkMorseSmaleComplex& filter) {
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
struct TTKTraits<ttkMorseSmaleComplex> {
    static constexpr std::string_view identifier = "ttkMorseSmaleComplex";
    static constexpr std::string_view displayName = "TTK MorseSmaleComplex";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 4> outports = {
        OutputData{"port0", "Critical Points", 0}, OutputData{"port1", "1-Separatrices", 1},
        OutputData{"port2", "2-Separatrices", 2}, OutputData{"port3", "Segmentation", 3}};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}},
        Group{"Input options", {"Scalar Field", "ForceInputOffsetScalarField", "Offset Field"}},
        Group{"Output options",
              {"ComputeCriticalPoints", "ComputeAscendingSeparatrices1",
               "ComputeDescendingSeparatrices1", "ComputeSaddleConnectors",
               "ComputeAscendingSeparatrices2", "ComputeDescendingSeparatrices2",
               "ComputeAscendingSegmentation", "ComputeDescendingSegmentation",
               "ComputeFinalSegmentation", "ReturnSaddleConnectors",
               "SaddleConnectorsPersistenceThreshold"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14, Wrapper15,
               Wrapper16>
        properties;
};

void registerttkMorseSmaleComplex(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkMorseSmaleComplex>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
