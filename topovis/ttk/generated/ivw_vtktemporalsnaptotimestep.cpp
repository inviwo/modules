#include "ivw_vtktemporalsnaptotimestep.h"

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
#include <vtkTemporalSnapToTimeStep.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkTemporalSnapToTimeStep& filter) {
        filter.SetSnapMode(property.get());
        return true;
    }
    OptionPropertyInt property{"SnapMode",
                               "SnapMode",
                               R"(Determine which time step to snap to.)"_help,
                               {{"Nearest", "Nearest", 0},
                                {"NextBelowOrEqual", "NextBelowOrEqual", 1},
                                {"NextAboveOrEqual", "NextAboveOrEqual", 2}},
                               0};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkTemporalSnapToTimeStep> {
    static constexpr std::string_view className = "vtkTemporalSnapToTimeStep";
    static constexpr std::string_view identifier = "TemporalSnapToTimeStep";
    static constexpr std::string_view displayName = "Temporal Snap-to-Time-Step";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataObject", -1, R"(This property specifies the input of the
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc =
        R"(This file modifies the time range or time steps of the
data without changing the data itself. The data is not
resampled by this filter, only the information
accompanying the data is modified.)";
};

void registervtkTemporalSnapToTimeStep(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkTemporalSnapToTimeStep>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
