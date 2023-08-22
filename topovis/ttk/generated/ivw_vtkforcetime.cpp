#include "ivw_vtkforcetime.h"

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
#include <vtkForceTime.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkForceTime& filter) {
        filter.SetIgnorePipelineTime(property.get());
        return true;
    }
    BoolProperty property{"IgnorePipelineTime", "IgnorePipelineTime",
                          R"(If set to 0, this filter will do nothing, only shallow copy the
input to the output. If set to 1, this filter will always request the
ForcedTime to the pipeline, ignoring time requests.)"_help,
                          true};
};

struct Wrapper1 {
    bool set(vtkForceTime& filter) {
        filter.SetForcedTime(property.get());
        return true;
    }
    DoubleProperty property{"ForcedTime",
                            "ForcedTime",
                            R"(This property specifies the time to request.
If the IgnorePipelineTime property is set, then this value will override any time request in the VTK pipeline.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkForceTime> {
    static constexpr std::string_view className = "vtkForceTime";
    static constexpr std::string_view identifier = "ForceTime";
    static constexpr std::string_view displayName = "Force Time";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1,
                  R"(This property specifies the input to the ForceTime filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;
    static constexpr std::string_view doc =
        R"(Filter used to explicitly request a specific time from the pipeline.)";
};

void registervtkForceTime(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkForceTime>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
