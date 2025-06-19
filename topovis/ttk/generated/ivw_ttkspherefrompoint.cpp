#include "ivw_ttkspherefrompoint.h"
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/vtk/processors/vtkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/ttk/util/ttkprocessorutils.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataObject.h>
#include <ttkSphereFromPoint.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetRadius(property.get());
        return true;
    }
    DoubleProperty property{"Radius",
                            "Radius",
                            R"(Sphere radius.)"_help,
                            0.5,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetThetaResolution(property.get());
        return true;
    }
    IntProperty property{"ThetaResolution",
                         "Theta Resolution",
                         R"(Theta resolution.)"_help,
                         20,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetStartTheta(property.get());
        return true;
    }
    IntProperty property{"StartTheta",
                         "Start Theta",
                         R"(Start theta.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{360, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetEndTheta(property.get());
        return true;
    }
    IntProperty property{"EndTheta",
                         "End Theta",
                         R"(End theta.)"_help,
                         360,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{360, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetPhiResolution(property.get());
        return true;
    }
    IntProperty property{"PhiResolution",
                         "Phi Resolution",
                         R"(Phi resolution.)"_help,
                         20,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetStartPhi(property.get());
        return true;
    }
    IntProperty property{"StartPhi",
                         "Start Phi",
                         R"(Start phi.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{360, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetEndPhi(property.get());
        return true;
    }
    IntProperty property{"EndPhi",
                         "End Phi",
                         R"(End phi.)"_help,
                         180,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{180, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper8 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber",
                         "Thread Number",
                         R"(The maximum number of threads.)"_help,
                         1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel",
                         "Debug Level",
                         R"(Debug level.)"_help,
                         3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(ttkSphereFromPoint& filter) {
        filter.SetCompactTriangulationCacheSize(property.get());
        return true;
    }
    DoubleProperty property{"CompactTriangulationCacheSize",
                            "Cache",
                            R"(Set the cache size for the compact triangulation as a
ratio with respect to the total cluster number.)"_help,
                            0.2,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper11 {
    bool set(ttkSphereFromPoint& filter) {
        filter.Modified();
        return true;
    }
    ButtonProperty property{"Debug_Execute", "Execute",
                            R"(Executes the filter with the last applied parameters, which is
handy to re-start pipeline execution from a specific element
without changing parameters.)"_help};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<ttkSphereFromPoint> {
    static constexpr std::string_view uri = "ttk.ttkSphereFromPoint";
    static constexpr std::string_view className = "ttkSphereFromPoint";
    static constexpr std::string_view identifier = "ttkSphereFromPoint";
    static constexpr std::string_view displayName = "TTK SphereFromPoint";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(Input point set.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc = R"ivw(TTK plugin that produces sphere-only glyphs.)ivw";
};

void registerttkSphereFromPoint(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkSphereFromPoint>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
