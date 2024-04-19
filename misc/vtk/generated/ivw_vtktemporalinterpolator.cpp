#include "ivw_vtktemporalinterpolator.h"
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
#include <vtkTemporalInterpolator.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkTemporalInterpolator& filter) {
        filter.SetDiscreteTimeStepInterval(property.get());
        return true;
    }
    DoubleProperty property{"DiscreteTimeStepInterval",
                            "DiscreteTimeStepInterval",
                            R"(If Discrete Time Step Interval is set to 0, then the
Temporal Interpolator will provide a continuous region of time on its
output. If set to anything else, then the output will define a finite
set of time points on its output, each spaced by the Discrete Time Step
Interval. The output will have (time range)/(discrete time step
interval) time steps. (Note that the time range is defined by the time
range of the data of the input filter, which may be different from
other pipeline objects or the range defined in the animation
inspector.) This is a useful option to use if you have a dataset with
one missing time step and wish to 'fill in' the missing data with an
interpolated value from the steps on either side.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkTemporalInterpolator> {
    static constexpr std::string_view uri = "vtk.vtkTemporalInterpolator";
    static constexpr std::string_view className = "vtkTemporalInterpolator";
    static constexpr std::string_view identifier = "TemporalInterpolator";
    static constexpr std::string_view displayName = "Temporal Interpolator";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataObject", -1, R"(This property specifies the input of the Temporal
Interpolator.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;

    static constexpr std::string_view doc = R"(The Temporal
Interpolator converts data that is defined at discrete
time steps to one that is defined over a continuum of time
by linearly interpolating the data's field data between
two adjacent time steps. The interpolated values are a
simple approximation and should not be interpreted as
anything more. The Temporal Interpolator assumes that the
topology between adjacent time steps does not
change.)";
};

void registervtkTemporalInterpolator(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkTemporalInterpolator>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
