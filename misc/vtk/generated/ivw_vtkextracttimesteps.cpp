#include "ivw_vtkextracttimesteps.h"
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
#include <vtkExtractTimeSteps.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkExtractTimeSteps& filter) {
        filter.SetUseRange(property.get());
        return true;
    }
    OptionPropertyInt property{
        "SelectionMode",
        "SelectionMode",
        R"(Extract time steps supports two ways to select the time steps to be extracted:

- **Select Time Steps**: Specify time steps to be extracted by their index

- **Select Time Range**: Specify the start and end time steps of a range by their index and optionally specify
a stride to optionally take every nth timestep in the range.)"_help,
        {{"Select Time Steps", "Select Time Steps", 0},
         {"Select Time Range", "Select Time Range", 1}},
        0};
};

struct Wrapper1 {
    bool set(vtkExtractTimeSteps& filter) {
        filter.SetRange(property.get(0), property.get(1));
        return true;
    }
    IntVec2Property property{
        "TimeStepRange",
        "TimeStepRange",
        R"(Select the beginning and end indices of the range of timesteps to extract.)"_help,
        ivec2{0, 1},
        std::pair{ivec2{0, 0}, ConstraintBehavior::Ignore},
        std::pair{ivec2{100, 100}, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkExtractTimeSteps& filter) {
        filter.SetTimeStepInterval(property.get());
        return true;
    }
    IntProperty property{
        "TimeStepInterval",
        "TimeStepInterval",
        R"(Select the stride to use when extracting the range of time steps (the n in every nth timestep).)"_help,
        1,
        std::pair{1, ConstraintBehavior::Ignore},
        std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkExtractTimeSteps& filter) {
        filter.SetTimeEstimationMode(property.get());
        return true;
    }
    OptionPropertyInt property{
        "ApproximationMode",
        "ApproximationMode",
        R"(If the scene's time does not match the time in any of the timesteps selected (likely because something
else in the scene has time steps at different times) this setting controls which timestep should
be returned by the filter.  The options are the previous time step, the nearest time step and the next
time step.)"_help,
        {{"Previous Time Step", "Previous Time Step", 0},
         {"Next Time Step", "Next Time Step", 1},
         {"Nearest Time Step", "Nearest Time Step", 2}},
        2};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkExtractTimeSteps> {
    static constexpr std::string_view uri = "vtk.vtkExtractTimeSteps";
    static constexpr std::string_view className = "vtkExtractTimeSteps";
    static constexpr std::string_view identifier = "ExtractTimeSteps";
    static constexpr std::string_view displayName = "Extract Time Steps";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataObject", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {Group{"Time Step Indices", {"TimeStepIndices"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;

    static constexpr std::string_view doc =
        R"(This filter extracts specified time steps from its input.)";
};

void registervtkExtractTimeSteps(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkExtractTimeSteps>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
