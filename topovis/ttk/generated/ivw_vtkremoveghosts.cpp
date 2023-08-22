#include "ivw_vtkremoveghosts.h"

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
#include <vtkRemoveGhosts.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkRemoveGhosts> {
    static constexpr std::string_view className = "vtkRemoveGhosts";
    static constexpr std::string_view identifier = "RemoveGhostInformation";
    static constexpr std::string_view displayName = "Remove Ghost Information";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkPolyData", -1, R"(This property specifies the input to the remove ghost
information filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;
    static constexpr std::string_view doc = R"(Removes ghost
cells and point data and cell data ghost arrays.)";
};

void registervtkRemoveGhosts(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkRemoveGhosts>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
