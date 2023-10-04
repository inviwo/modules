#include "ivw_vtktexturemaptosphere.h"

#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/ttk/processors/ttkgenericprocessor.h>
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
#include <vtkTextureMapToSphere.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkTextureMapToSphere& filter) {
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

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkTextureMapToSphere> {
    static constexpr std::string_view className = "vtkTextureMapToSphere";
    static constexpr std::string_view identifier = "TextureMapToSphere";
    static constexpr std::string_view displayName = "Texture Map to Sphere";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(Set the input to the Texture Map to Sphere
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc =
        R"(This is a filter that generates 2D texture coordinates by
mapping input dataset points onto a sphere. The sphere is
generated automatically. The sphere is generated
automatically by computing the center i.e. averaged
coordinates, of the sphere. Note that the generated
texture coordinates range between (0,1). The s-coordinate
lies in the angular direction around the z-axis, measured
counter-clockwise from the x-axis. The t-coordinate lies
in the angular direction measured down from the north pole
towards the south pole.)";
};

void registervtkTextureMapToSphere(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkTextureMapToSphere>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
