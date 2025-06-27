#include "ivw_vtktemporalshiftscale.h"
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
#include <vtkTemporalShiftScale.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkTemporalShiftScale& filter) {
        filter.SetPreShift(property.get());
        return true;
    }
    DoubleProperty property{"PreShift",
                            "PreShift",
                            R"(Apply a translation to the data before scaling. To
convert T{5,100} to T{0,1} use Preshift=-5, Scale=1/95, PostShift=0 To
convert T{5,105} to T{5,10} use Preshift=-5, Scale=5/100,
PostShift=5)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkTemporalShiftScale& filter) {
        filter.SetPostShift(property.get());
        return true;
    }
    DoubleProperty property{"PostShift",
                            "PostShift",
                            R"(The amount of time the input is shifted.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkTemporalShiftScale& filter) {
        filter.SetScale(property.get());
        return true;
    }
    DoubleProperty property{"Scale",
                            "Scale",
                            R"(The factor by which the input time is
scaled.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkTemporalShiftScale& filter) {
        filter.SetPeriodic(property.get());
        return true;
    }
    BoolProperty property{"Periodic", "Periodic",
                          R"(If Periodic is true, requests for time will be wrapped
around so that the source appears to be a periodic time source. If data
exists for times {0,N-1}, setting periodic to true will cause time 0 to
be produced when time N, 2N, 2N etc is requested. This effectively
gives the source the ability to generate time data indefinitely in a
loop. When combined with Shift/Scale, the time becomes periodic in the
shifted and scaled time frame of reference. Note: Since the input time
may not start at zero, the wrapping of time from the end of one period
to the start of the next, will subtract the initial time - a source
with T{5..6} repeated periodically will have output time {5..6..7..8}
etc.)"_help,
                          false};
};

struct Wrapper4 {
    bool set(vtkTemporalShiftScale& filter) {
        filter.SetPeriodicEndCorrection(property.get());
        return true;
    }
    BoolProperty property{"PeriodicEndCorrection", "PeriodicEndCorrection",
                          R"(If Periodic time is enabled, this flag determines if the
last time step is the same as the first. If PeriodicEndCorrection is
true, then it is assumed that the input data goes from 0-1 (or whatever
scaled/shifted actual time) and time 1 is the same as time 0 so that
steps will be 0,1,2,3...N,1,2,3...N,1,2,3 where step N is the same as 0
and step 0 is not repeated. When this flag is false the data is assumed
to be literal and output is of the form 0,1,2,3...N,0,1,2,3... By
default this flag is ON)"_help,
                          true};
};

struct Wrapper5 {
    bool set(vtkTemporalShiftScale& filter) {
        filter.SetMaximumNumberOfPeriods(property.get());
        return true;
    }
    DoubleProperty property{"MaximumNumberOfPeriods",
                            "MaximumNumberOfPeriods",
                            R"(If Periodic time is enabled, this controls how many time
periods time is reported for. A filter cannot output an infinite number
of time steps and therefore a finite number of periods is generated
when reporting time.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkTemporalShiftScale> {
    static constexpr std::string_view uri = "vtk.vtkTemporalShiftScale";
    static constexpr std::string_view className = "vtkTemporalShiftScale";
    static constexpr std::string_view identifier = "TemporalShiftScale";
    static constexpr std::string_view displayName = "Temporal Shift Scale";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataObject", -1, R"(The input to the Temporal Shift Scale
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;

    static constexpr std::string_view doc = R"ivw(The Temporal
Shift Scale filter linearly transforms the time values of
a pipeline object by applying a shift and then scale.
Given a data at time t on the input, it will be
transformed to time t*Shift + Scale on the output.
Inversely, if this filter has a request for time t, it
will request time (t-Shift)/Scale on its
input.)ivw";
};

void registervtkTemporalShiftScale(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkTemporalShiftScale>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
