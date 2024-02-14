#include "ivw_vtkpassarrays.h"
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
#include <vtkPassArrays.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkPassArrays& filter) {
        filter.SetUseFieldTypes(property.get());
        return true;
    }
    BoolProperty property{"UseFieldTypes", "UseFieldTypes",
                          R"(This hidden property must always be set to 1 for this
proxy to work.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkPassArrays> {
    static constexpr std::string_view uri = "vtk.vtkPassArrays";
    static constexpr std::string_view className = "vtkPassArrays";
    static constexpr std::string_view identifier = "PassArrays";
    static constexpr std::string_view displayName = "Pass Arrays";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataObject", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc =
        R"(The Pass Arrays filter makes a shallow copy of the output
data object from the input data object except for passing
only the arrays specified to the output from the
input.)";
};

void registervtkPassArrays(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkPassArrays>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
