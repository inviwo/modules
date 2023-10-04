#include "ivw_vtkmultiblockdatagroupfilter.h"

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
#include <vtkMultiBlockDataGroupFilter.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkMultiBlockDataGroupFilter> {
    static constexpr std::string_view className = "vtkMultiBlockDataGroupFilter";
    static constexpr std::string_view identifier = "GroupDataSets";
    static constexpr std::string_view displayName = "Group Datasets";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataObject", -1, R"(This property indicates the the inputs to the Group
Datasets filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;
    static constexpr std::string_view doc = R"(Groups multiple datasets to create a multiblock
dataset)";
};

void registervtkMultiBlockDataGroupFilter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkMultiBlockDataGroupFilter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
