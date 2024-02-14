#include "ivw_vtkrotationalextrusionfilter.h"
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/vtk/processors/vtkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataObject.h>
#include <vtkRotationalExtrusionFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkRotationalExtrusionFilter& filter) {
        filter.SetResolution(property.get());
        return true;
    }
    IntProperty property{"Resolution",
                         "Resolution",
                         R"(The value of this property controls the number of
intermediate node points used in performing the sweep (rotating from 0
degrees to the value specified by the Angle property.)"_help,
                         12,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkRotationalExtrusionFilter& filter) {
        filter.SetCapping(property.get());
        return true;
    }
    BoolProperty property{"Capping", "Capping",
                          R"(If this property is set to 1, the open ends of the swept
surface will be capped with a copy of the input dataset. This works
property if the input is a 2D surface composed of filled polygons. If
the input dataset is a closed solid (e.g., a sphere), then either two
copies of the dataset will be drawn or no surface will be drawn. No
surface is drawn if either this property is set to 0 or if the two
surfaces would occupy exactly the same 3D space (i.e., the Angle
property's value is a multiple of 360, and the values of the
Translation and DeltaRadius properties are 0).)"_help,
                          true};
};

struct Wrapper2 {
    bool set(vtkRotationalExtrusionFilter& filter) {
        filter.SetAngle(property.get());
        return true;
    }
    DoubleProperty property{"Angle",
                            "Angle",
                            R"(This property specifies the angle of rotation in
degrees. The surface is swept from 0 to the value of this
property.)"_help,
                            360.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkRotationalExtrusionFilter& filter) {
        filter.SetTranslation(property.get());
        return true;
    }
    DoubleProperty property{"Translation",
                            "Translation",
                            R"(The value of this property specifies the total amount of
translation along the Z axis during the sweep process. Specifying a
non-zero value for this property allows you to create a corkscrew
(value of DeltaRadius > 0) or spring effect.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(vtkRotationalExtrusionFilter& filter) {
        filter.SetDeltaRadius(property.get());
        return true;
    }
    DoubleProperty property{"DeltaRadius",
                            "DeltaRadius",
                            R"(The value of this property specifies the change in
radius during the sweep process.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkRotationalExtrusionFilter> {
    static constexpr std::string_view uri = "vtk.vtkRotationalExtrusionFilter";
    static constexpr std::string_view className = "vtkRotationalExtrusionFilter";
    static constexpr std::string_view identifier = "RotationalExtrusionFilter";
    static constexpr std::string_view displayName = "Rotational Extrusion";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkPolyData", -1, R"(This property specifies the input to the Rotational
Extrusion filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;
    static constexpr std::string_view doc = R"(The Rotational Extrusion filter forms a surface by
rotating the input about the Z axis. This filter is
intended to operate on 2D polygonal data. It produces
polygonal output.)";
};

void registervtkRotationalExtrusionFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkRotationalExtrusionFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
