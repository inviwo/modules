#include "ivw_vtkimageclip.h"
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
#include <vtkImageClip.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkImageClip> {
    static constexpr std::string_view uri = "vtk.vtkImageClip";
    static constexpr std::string_view className = "vtkImageClip";
    static constexpr std::string_view identifier = "ImageClip";
    static constexpr std::string_view displayName = "Crop";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkImageData", -1, R"(This property specifies the input to the Crop
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;
    static constexpr std::string_view doc = R"(The Crop filter
extracts an area/volume of interest from a 2D image or a
3D volume by allowing the user to specify the minimum and
maximum extents of each dimension of the data. Both the
input and output of this filter are uniform rectilinear
data.)";
};

void registervtkImageClip(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkImageClip>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
