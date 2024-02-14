#include "ivw_vtkappendarclength.h"
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
#include <vtkAppendArcLength.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkAppendArcLength> {
    static constexpr std::string_view uri = "vtk.vtkAppendArcLength";
    static constexpr std::string_view className = "vtkAppendArcLength";
    static constexpr std::string_view identifier = "AppendArcLength";
    static constexpr std::string_view displayName = "Append Arc-Length";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkPolyData", -1, R"(The input.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;
    static constexpr std::string_view doc = R"(vtkAppendArcLength is used for filter such as
plot-over-line. In such cases, we need to add an attribute
array that is the arc_length over the length of the probed
line. That's when vtkAppendArcLength can be used. It adds
a new point-data array named "arc_length" with the
computed arc length for each of the polylines in the
input. For all other cell types, the arc length is set to
0.)";
};

void registervtkAppendArcLength(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkAppendArcLength>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
