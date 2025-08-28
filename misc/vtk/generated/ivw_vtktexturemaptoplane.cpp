#include "ivw_vtktexturemaptoplane.h"
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
#include <vtkTextureMapToPlane.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkTextureMapToPlane& filter) {
        filter.SetOrigin(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{"Origin",
                                "Origin",
                                R"(This property specifies the 3D coordinates for the
origin of the plane. Set all to zero if you want to use automatic
generation.)"_help,
                                dvec3{0.0, 0.0, 0.0},
                                std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkTextureMapToPlane& filter) {
        filter.SetPoint1(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{"Point1",
                                "Point1",
                                R"(This property specifies the 3D coordinates for
Point1 of the plane. Set all to zero if you want to use automatic
generation.)"_help,
                                dvec3{0.0, 0.0, 0.0},
                                std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkTextureMapToPlane& filter) {
        filter.SetPoint2(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{"Point2",
                                "Point2",
                                R"(This property specifies the 3D coordinates for
Point2 of the plane. Set all to zero if you want to use automatic
generation.)"_help,
                                dvec3{0.0, 0.0, 0.0},
                                std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkTextureMapToPlane& filter) {
        filter.SetAutomaticPlaneGeneration(property.get());
        return true;
    }
    BoolProperty property{"Automatic", "Automatic",
                          R"(If set the plane values will be automatically generated.
Note that for this to work all the Origin, Point1 and Point2 must all
be set to zero.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkTextureMapToPlane> {
    static constexpr std::string_view uri = "vtk.vtkTextureMapToPlane";
    static constexpr std::string_view className = "vtkTextureMapToPlane";
    static constexpr std::string_view identifier = "TextureMapToPlane";
    static constexpr std::string_view displayName = "Texture Map to Plane";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkDataSet",
                  .numComp = -1,
                  .doc = R"(Set the input to the Texture Map to Plane
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;

    static constexpr std::string_view doc =
        R"ivw(TextureMapToPlane is a filter that generates 2D texture
coordinates by mapping input dataset points onto a plane.
The plane is generated automatically. A least squares
method is used to generate the plane
automatically.)ivw";
};

void registervtkTextureMapToPlane(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkTextureMapToPlane>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
