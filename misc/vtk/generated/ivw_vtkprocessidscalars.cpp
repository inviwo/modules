#include "ivw_vtkprocessidscalars.h"
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
#include <vtkProcessIdScalars.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkProcessIdScalars& filter) {
        filter.SetRandomMode(property.get());
        return true;
    }
    BoolProperty property{"RandomMode", "RandomMode",
                          R"(The value of this property determines whether to use
random id values for the various pieces. If set to 1, the unique value
per piece will be chosen at random; otherwise the unique value will
match the id of the process.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkProcessIdScalars> {
    static constexpr std::string_view uri = "vtk.vtkProcessIdScalars";
    static constexpr std::string_view className = "vtkProcessIdScalars";
    static constexpr std::string_view identifier = "ProcessIdScalars";
    static constexpr std::string_view displayName = "Process Id Scalars";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(This property specifies the input to the Process Id
Scalars filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc = R"(The
Process Id Scalars filter assigns a unique scalar value to
each piece of the input according to which processor it
resides on. This filter operates on any type of data when
ParaView is run in parallel. It is useful for determining
whether your data is load-balanced across the processors
being used. The output data set type is the same as that
of the input.)";
};

void registervtkProcessIdScalars(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkProcessIdScalars>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
