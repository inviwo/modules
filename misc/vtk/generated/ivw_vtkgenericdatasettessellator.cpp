#include "ivw_vtkgenericdatasettessellator.h"
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
#include <vtkGenericDataSetTessellator.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkGenericDataSetTessellator> {
    static constexpr std::string_view uri = "vtk.vtkGenericDataSetTessellator";
    static constexpr std::string_view className = "vtkGenericDataSetTessellator";
    static constexpr std::string_view identifier = "GenericTessellator";
    static constexpr std::string_view displayName = "Tessellate Generic Dataset";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkGenericDataSet", -1, R"(Set the input to the Generic Tessellator
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;

    static constexpr std::string_view doc = R"(Tessellate
a higher-order dataset.)";
};

void registervtkGenericDataSetTessellator(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkGenericDataSetTessellator>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
