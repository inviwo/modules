#include "ivw_vtkimagegradientmagnitude.h"
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
#include <vtkImageGradientMagnitude.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkImageGradientMagnitude& filter) {
        filter.SetDimensionality(property.get());
        return true;
    }
    OptionPropertyInt property{"Dimensionality",
                               "Dimensionality",
                               R"(This property indicates whether to compute the gradient
magnitude in two or three dimensions. If computing the gradient
magnitude in 2D, the gradients in X and Y are used for computing the
gradient magnitude.)"_help,
                               {{"Two", "Two", 2}, {"Three", "Three", 3}},
                               1};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkImageGradientMagnitude> {
    static constexpr std::string_view uri = "vtk.vtkImageGradientMagnitude";
    static constexpr std::string_view className = "vtkImageGradientMagnitude";
    static constexpr std::string_view identifier = "GradientMagnitude";
    static constexpr std::string_view displayName = "Gradient Magnitude";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkImageData",
                  .numComp = 1,
                  .doc = R"(This property specifies the input to the Gradient
Magnitude filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;

    static constexpr std::string_view doc = R"ivw(The Gradient
Magnitude filter computes the magnitude of the gradient
vector at each point in an image or volume. This filter
operates on uniform rectilinear (image) data and produces
image data output.)ivw";
};

void registervtkImageGradientMagnitude(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkImageGradientMagnitude>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
