#include "ivw_vtkpassthrough.h"

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
#include <vtkPassThrough.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkPassThrough& filter) {
        filter.SetAllowNullInput(property.get());
        return true;
    }
    BoolProperty property{"AllowNullInput", "AllowNullInput",
                          R"(Allow the filter to execute successful, producing an empty polydata,
when the input is not specified.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkPassThrough> {
    static constexpr std::string_view className = "vtkPassThrough";
    static constexpr std::string_view identifier = "PipelineConnection";
    static constexpr std::string_view displayName = "Pipeline Connection";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc =
        R"(This proxy provides UI for selecting an existing pipeline connection.)";
};

void registervtkPassThrough(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkPassThrough>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
