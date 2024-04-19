#include "ivw_ttkcinemaimaging.h"
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
#include <ttkCinemaImaging.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetResolution(property.get(0), property.get(1));
        return true;
    }
    IntVec2Property property{"Resolution",
                             "Resolution",
                             R"(Image Resolution)"_help,
                             ivec2{256, 256},
                             std::pair{ivec2{0, 0}, ConstraintBehavior::Ignore},
                             std::pair{ivec2{100, 100}, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetBackend(property.get());
        return true;
    }
    OptionPropertyInt property{
        "Backend",
        "Backend",
        R"(Rendering Backend)"_help,
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
        R"(Use orthographic or perspective camera projection.)"_help,
        {{"Orthographic", "Orthographic", 0}, {"Perspective", "Perspective", 1}},
        0};
};

struct Wrapper3 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetAutoFocalPoint(property.get());
        return true;
    }
    BoolProperty property{"AutoFocalPoint", "AutoFocalPoint",
                          R"(Automatically set focus to sampling grid center.)"_help, true};
};

struct Wrapper4 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetFocalPoint(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{"FocalPoint",
                                "FocalPoint",
                                R"(Camera FocalPoint.)"_help,
                                dvec3{0.0, 0.0, 0.0},
                                std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetAutoNearFar(property.get());
        return true;
    }
    BoolProperty property{
        "AutoNearFar", "AutoNearFar",
        R"(Automatically set camera near-far plane settings based on grid and object bounds.)"_help,
        true};
};

struct Wrapper6 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetNearFar(property.get(0), property.get(1));
        return true;
    }
    DoubleVec2Property property{"NearFar",
                                "NearFar",
                                R"(Near/Far plane settings.)"_help,
                                dvec2{0.1, 1.0},
                                std::pair{dvec2{0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec2{100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetAutoHeight(property.get());
        return true;
    }
    BoolProperty property{"AutoHeight", "AutoHeight",
                          R"(Automatically set camera height to sampling grid diameter.)"_help,
                          true};
};

struct Wrapper8 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetHeight(property.get());
        return true;
    }
    DoubleProperty property{"Height",
                            "Height",
                            R"(Height of the camera in world coordinates.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetAngle(property.get());
        return true;
    }
    DoubleProperty property{"Angle",
                            "Angle",
                            R"(Camera Angle.)"_help,
                            90.0,
                            std::pair{1.0, ConstraintBehavior::Ignore},
                            std::pair{180.0, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(ttkCinemaImaging& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper11 {
    bool set(ttkCinemaImaging& filter) {
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

struct Wrapper12 {
    bool set(ttkCinemaImaging& filter) {
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

struct Wrapper13 {
    bool set(ttkCinemaImaging& filter) {
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

struct Wrapper14 {
    bool set(ttkCinemaImaging& filter) {
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
struct VTKTraits<ttkCinemaImaging> {
    static constexpr std::string_view uri = "ttk.ttkCinemaImaging";
    static constexpr std::string_view className = "ttkCinemaImaging";
    static constexpr std::string_view identifier = "ttkCinemaImaging";
    static constexpr std::string_view displayName = "TTK CinemaImaging";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Dataset", "vtkMultiBlockDataSet", -1,
                  R"(Triangulation that is going to be depicted.)"},
        InputData{"SamplingGrid", "vtkPointSet", -1,
                  R"(vtkPointSet that represents the camera sampling locations.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Render Options", {"Resolution", "Backend"}},
        Group{"Camera Options",
              {"ProjectionMode", "AutoFocalPoint", "FocalPoint", "AutoNearFar", "NearFar",
               "AutoHeight", "Height", "Angle"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"(This filter takes images of a vtkDataObject (first input) from angles specified on a vtkPointSet (second input). Each image will be a block of a vtkMultiBlockDataSet where block order corresponds to point order. Each sample point can optionally have vtkDoubleArrays to override the default rendering parameters, i.e, the resolution, camera direction, clipping planes, and viewport height.

Online examples:

- https://topology-tool-kit.github.io/examples/geometryApproximation/)";
};

void registerttkCinemaImaging(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkCinemaImaging>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
