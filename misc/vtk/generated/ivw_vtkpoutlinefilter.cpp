#include "ivw_vtkpoutlinefilter.h"
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
#include <vtkPOutlineFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkPOutlineFilter> {
    static constexpr std::string_view uri = "vtk.vtkPOutlineFilter";
    static constexpr std::string_view className = "vtkPOutlineFilter";
    static constexpr std::string_view identifier = "OutlineFilter";
    static constexpr std::string_view displayName = "Outline";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(This property specifies the input to the Outline
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;
    static constexpr std::string_view doc = R"(The Outline filter
generates an axis-aligned bounding box for the input
dataset. This filter operates on any type of dataset and
produces polygonal output.)";
};

void registervtkPOutlineFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkPOutlineFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
