#include "ivwwrapcinemaimaging.h"

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
#include "ttkCinemaImaging.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetResolution(property.get(0), property.get(1));
        return true;
    }
    IntVec2Property property{"Resolution", "Resolution", ivec2{256, 256},
                             std::pair{ivec2{0}, ConstraintBehavior::Ignore},
                             std::pair{ivec2{100}, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetBackend(property.get());
        return true;
    }
    OptionPropertyInt property{
        "Backend",
        "Backend",
        {{"VTK OpenGL", "VTK OpenGL", 0}, {"Embree", "Embree", 1}, {"Native", "Native", 2}},
        0};
};

struct Wrapper2 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetProjectionMode(property.get());
        return true;
    }
    OptionPropertyInt property{
        "ProjectionMode",
        "ProjectionMode",
        {{"Orthographic", "Orthographic", 0}, {"Perspective", "Perspective", 1}},
        0};
};

struct Wrapper3 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetAutoFocalPoint(property.get());
        return true;
    }
    BoolProperty property{"AutoFocalPoint", "AutoFocalPoint", true};
};

struct Wrapper4 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetFocalPoint(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{"FocalPoint", "FocalPoint", dvec3{0.0, 0.0, 0.0},
                                std::pair{dvec3{0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec3{100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetAutoNearFar(property.get());
        return true;
    }
    BoolProperty property{"AutoNearFar", "AutoNearFar", true};
};

struct Wrapper6 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetNearFar(property.get(0), property.get(1));
        return true;
    }
    DoubleVec2Property property{"NearFar", "NearFar", dvec2{0.1, 1.0},
                                std::pair{dvec2{0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec2{100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetAutoHeight(property.get());
        return true;
    }
    BoolProperty property{"AutoHeight", "AutoHeight", true};
};

struct Wrapper8 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetHeight(property.get());
        return true;
    }
    DoubleProperty property{"Height", "Height", 1.0, std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetAngle(property.get());
        return true;
    }
    DoubleProperty property{"Angle", "Angle", 90.0, std::pair{1.0, ConstraintBehavior::Ignore},
                            std::pair{180.0, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper11 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper12 {
    bool set(ttkCinemaImaging& filter) {
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
struct TTKTraits<ttkCinemaImaging> {
    static constexpr std::string_view identifier = "ttkCinemaImaging";
    static constexpr std::string_view displayName = "TTK CinemaImaging";
    inline static std::array<InputData, 2> inports = {
        InputData{"Dataset", "vtkMultiBlockDataSet", -1},
        InputData{"SamplingGrid", "vtkPointSet", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Render Options", {"Resolution", "Backend"}},
        Group{"Camera Options",
              {"ProjectionMode", "AutoFocalPoint", "FocalPoint", "AutoNearFar", "NearFar",
               "AutoHeight", "Height", "Angle"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12>
        properties;
};

void registerttkCinemaImaging(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkCinemaImaging>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
