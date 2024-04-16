#include "ivw_vtktexturemaptocylinder.h"
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
#include <vtkTextureMapToCylinder.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkTextureMapToCylinder& filter) {
        filter.SetPreventSeam(property.get());
        return true;
    }
    BoolProperty property{"PreventSeam", "PreventSeam",
                          R"(Control how the texture coordinates are generated. If
Prevent Seam is set, the s-coordinate ranges from 0->1 and 1->0
corresponding to the theta angle variation between 0->180 and
180->0 degrees. Otherwise, the s-coordinate ranges from 0->1
between 0->360 degrees.)"_help,
                          true};
};

struct Wrapper1 {
    bool set(vtkTextureMapToCylinder& filter) {
        filter.SetAutomaticCylinderGeneration(property.get());
        return true;
    }
    BoolProperty property{
        "GenerateCylinderAutomatically", "GenerateCylinderAutomatically",
        R"(When set, the filter will try to determine the size and orientation of the cylinder
used for texture mapping using data bounds.)"_help,
        true};
};

struct Wrapper2 {
    bool set(vtkTextureMapToCylinder& filter) {
        filter.SetPoint1(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{
        "Point1",
        "Point1",
        R"(When **GenerateCylinderAutomatically** is not set, specify the first point defining
the axis of the cylinder through its center.)"_help,
        dvec3{0.0, 0.0, 0.0},
        std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
        std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkTextureMapToCylinder& filter) {
        filter.SetPoint2(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{
        "Point2",
        "Point2",
        R"(When **GenerateCylinderAutomatically** is not set, specify the second point defining
the axis of the cylinder through its center.)"_help,
        dvec3{0.0, 0.0, 0.0},
        std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
        std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkTextureMapToCylinder> {
    static constexpr std::string_view uri = "vtk.vtkTextureMapToCylinder";
    static constexpr std::string_view className = "vtkTextureMapToCylinder";
    static constexpr std::string_view identifier = "TextureMapToCylinder";
    static constexpr std::string_view displayName = "Texture Map to Cylinder";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(Set the input to the Texture Map to Cylinder
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;

    static constexpr std::string_view doc =
        R"(This is a filter that generates 2D texture coordinates by
mapping input dataset points onto a cylinder. The cylinder
is generated automatically. The cylinder is generated
automatically by computing the axis of the cylinder. Note
that the generated texture coordinates for the
s-coordinate ranges from (0-1) (corresponding to angle of
0->360 around axis), while the mapping of the
t-coordinate is controlled by the projection of points
along the axis.)";
};

void registervtkTextureMapToCylinder(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkTextureMapToCylinder>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
