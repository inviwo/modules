#include "ivw_vtkcompositecutter.h"
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
#include <vtkCompositeCutter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkCompositeCutter> {
    static constexpr std::string_view uri = "vtk.vtkCompositeCutter";
    static constexpr std::string_view className = "vtkCompositeCutter";
    static constexpr std::string_view identifier = "CutEverything";
    static constexpr std::string_view displayName = "Slice (demand-driven-composite)";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataObject", -1, R"(This property specifies the input to the Slice
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;

    static constexpr std::string_view doc = R"ivw(This filter
extracts the portion of the input dataset that lies along
the specified plane. The Slice filter takes any type of
dataset as input. The output of this filter is polygonal
data.)ivw";
};

void registervtkCompositeCutter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkCompositeCutter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
