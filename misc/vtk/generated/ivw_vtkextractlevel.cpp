#include "ivw_vtkextractlevel.h"
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
#include <vtkExtractLevel.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkExtractLevel> {
    static constexpr std::string_view uri = "vtk.vtkExtractLevel";
    static constexpr std::string_view className = "vtkExtractLevel";
    static constexpr std::string_view identifier = "ExtractHierarchicalLevel";
    static constexpr std::string_view displayName = "Extract Level";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkUniformGridAMR", -1, R"(This property specifies the input to the Extract Group
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;
    static constexpr std::string_view doc = R"(This filter extracts a range
of levels from a hierarchical dataset)";
};

void registervtkExtractLevel(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkExtractLevel>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
