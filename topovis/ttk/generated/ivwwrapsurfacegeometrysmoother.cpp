#include "ivwwrapsurfacegeometrysmoother.h"

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
#include "ttkSurfaceGeometrySmoother.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkSurfaceGeometrySmoother& filter) {
        filter.SetNumberOfIterations(property.get());
        return true;
    }
    IntProperty property{"NumberOfIterations", "Iteration Number", 1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkSurfaceGeometrySmoother& filter) {
        filter.SetForceIdentifiersField(property.get());
        return true;
    }
    BoolProperty property{"ForceIdentifiersField", "Force Vertex Identifiers Field", false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkSurfaceGeometrySmoother& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"VertexIdsField", "Identifiers Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkSurfaceGeometrySmoother& filter) {
        filter.SetUseMaskScalarField(property.get());
        return true;
    }
    BoolProperty property{"UseMaskScalarField", "Use a Scalar Field as Mask", true};
};

struct Wrapper4 {
    bool set(ttkSurfaceGeometrySmoother& filter) {
        filter.SetForceInputMaskScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputMaskScalarField", "Force Input Mask Scalar Field", false};
};

struct Wrapper5 : FieldSelection {
    bool set(ttkSurfaceGeometrySmoother& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"InputMaskNew", "Mask Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper6 {
    bool set(ttkSurfaceGeometrySmoother& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper7 {
    bool set(ttkSurfaceGeometrySmoother& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkSurfaceGeometrySmoother& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkSurfaceGeometrySmoother& filter) {
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
struct TTKTraits<ttkSurfaceGeometrySmoother> {
    static constexpr std::string_view identifier = "ttkSurfaceGeometrySmoother";
    static constexpr std::string_view displayName = "TTK SurfaceGeometrySmoother";
    inline static std::array<InputData, 2> inports = {InputData{"Input", "vtkPointSet", 1},
                                                      InputData{"Surface", "vtkPointSet", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Input options",
              {"Input", "NumberOfIterations", "ForceIdentifiersField", "VertexIdsField",
               "UseMaskScalarField", "ForceInputMaskScalarField", "InputMaskNew"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
};

void registerttkSurfaceGeometrySmoother(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkSurfaceGeometrySmoother>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo