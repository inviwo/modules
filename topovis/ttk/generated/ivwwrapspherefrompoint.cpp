#include "ivwwrapspherefrompoint.h"

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
#include "ttkSphereFromPoint.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetRadius(property.get());
        return true;
    }
    DoubleProperty property{"Radius", "Radius", 0.5, std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetThetaResolution(property.get());
        return true;
    }
    IntProperty property{"ThetaResolution", "Theta Resolution", 20,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetStartTheta(property.get());
        return true;
    }
    IntProperty property{"StartTheta", "Start Theta", 0, std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{360, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetEndTheta(property.get());
        return true;
    }
    IntProperty property{"EndTheta", "End Theta", 360, std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{360, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetPhiResolution(property.get());
        return true;
    }
    IntProperty property{"PhiResolution", "Phi Resolution", 20,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetStartPhi(property.get());
        return true;
    }
    IntProperty property{"StartPhi", "Start Phi", 0, std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{360, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetEndPhi(property.get());
        return true;
    }
    IntProperty property{"EndPhi", "End Phi", 180, std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{180, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper8 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(ttkSphereFromPoint& filter) {
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
struct TTKTraits<ttkSphereFromPoint> {
    static constexpr std::string_view identifier = "ttkSphereFromPoint";
    static constexpr std::string_view displayName = "TTK SphereFromPoint";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10>
        properties;
};

void registerttkSphereFromPoint(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkSphereFromPoint>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
