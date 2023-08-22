#include "ivw_vtkhypertreegridtounstructuredgrid.h"

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
#include <vtkHyperTreeGridToUnstructuredGrid.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkHyperTreeGridToUnstructuredGrid> {
    static constexpr std::string_view className = "vtkHyperTreeGridToUnstructuredGrid";
    static constexpr std::string_view identifier = "HyperTreeGridToUnstructuredGrid";
    static constexpr std::string_view displayName = "HyperTreeGrid To UnstructuredGrid";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkHyperTreeGrid", -1, R"(This property specifies the input to the converter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;
    static constexpr std::string_view doc =
        R"(This filter converts vtkHyperTreeGrid data to vtkUnstructuredGrid. The converted output consumes much more memory but is compatible with most of the standard filters.)";
};

void registervtkHyperTreeGridToUnstructuredGrid(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkHyperTreeGridToUnstructuredGrid>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
