#include "ivw_ttkquadrangulationsubdivision.h"

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
#include "ttkQuadrangulationSubdivision.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetSubdivisionLevel(property.get());
        return true;
    }
    IntProperty property{"SubdivisionLevel", "Level of subdivisions", 1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetRelaxationIterations(property.get());
        return true;
    }
    IntProperty property{"RelaxationIterations", "Number of relaxation iterations", 10,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetLockInputExtrema(property.get());
        return true;
    }
    BoolProperty property{"LockInputExtrema", "Lock extraordinary vertices", false};
};

struct Wrapper3 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetLockAllInputVertices(property.get());
        return true;
    }
    BoolProperty property{"LockAllInputVertices", "Lock all coarse vertices", false};
};

struct Wrapper4 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetQuadStatistics(property.get());
        return true;
    }
    BoolProperty property{"QuadStatistics", "Quad Statistics", false};
};

struct Wrapper5 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetShowResError(property.get());
        return true;
    }
    BoolProperty property{"ShowResError", "Force output despite warnings", false};
};

struct Wrapper6 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetHausdorffLevel(property.get());
        return true;
    }
    DoubleProperty property{"HausdorffLevel", "Hausdorff Warning Level", 200.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper8 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkQuadrangulationSubdivision& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(ttkQuadrangulationSubdivision& filter) {
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
struct TTKTraits<ttkQuadrangulationSubdivision> {
    static constexpr std::string_view identifier = "ttkQuadrangulationSubdivision";
    static constexpr std::string_view displayName = "TTK QuadrangulationSubdivision";
    inline static std::array<InputData, 2> inports = {InputData{"mesh", "vtkDataSet", -1},
                                                      InputData{"quadrangles", "vtkPolyData", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute", "HausdorffLevel", "ShowResError"}},
        Group{"Input options",
              {"SubdivisionLevel", "RelaxationIterations", "LockInputExtrema",
               "LockAllInputVertices", "QuadStatistics"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10>
        properties;
};

void registerttkQuadrangulationSubdivision(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkQuadrangulationSubdivision>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
