#include "ivw_vtktemporalstatistics.h"
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
#include <vtkTemporalStatistics.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkTemporalStatistics& filter) {
        filter.SetComputeAverage(property.get());
        return true;
    }
    BoolProperty property{"ComputeAverage", "ComputeAverage",
                          R"(Compute the average of each point and cell variable over
time.)"_help,
                          true};
};

struct Wrapper1 {
    bool set(vtkTemporalStatistics& filter) {
        filter.SetComputeMinimum(property.get());
        return true;
    }
    BoolProperty property{"ComputeMinimum", "ComputeMinimum",
                          R"(Compute the minimum of each point and cell variable over
time.)"_help,
                          true};
};

struct Wrapper2 {
    bool set(vtkTemporalStatistics& filter) {
        filter.SetComputeMaximum(property.get());
        return true;
    }
    BoolProperty property{"ComputeMaximum", "ComputeMaximum",
                          R"(Compute the maximum of each point and cell variable over
time.)"_help,
                          true};
};

struct Wrapper3 {
    bool set(vtkTemporalStatistics& filter) {
        filter.SetComputeStandardDeviation(property.get());
        return true;
    }
    BoolProperty property{"ComputeStandardDeviation", "ComputeStandardDeviation",
                          R"(Compute the standard deviation of each point and cell
variable over time.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkTemporalStatistics> {
    static constexpr std::string_view uri = "vtk.vtkTemporalStatistics";
    static constexpr std::string_view className = "vtkTemporalStatistics";
    static constexpr std::string_view identifier = "TemporalStatistics";
    static constexpr std::string_view displayName = "Temporal Statistics";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(Set the input to the Temporal Statistics
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;

    static constexpr std::string_view doc = R"(Given an input
that changes over time, vtkTemporalStatistics looks at the
data for each time step and computes some statistical
information of how a point or cell variable changes over
time. For example, vtkTemporalStatistics can compute the
average value of "pressure" over time of each point. Note
that this filter will require the upstream filter to be
run on every time step that it reports that it can
compute. This may be a time consuming operation.
vtkTemporalStatistics ignores the temporal spacing. Each
timestep will be weighted the same regardless of how long
of an interval it is to the next timestep. Thus, the
average statistic may be quite different from an
integration of the variable if the time spacing
varies.)";
};

void registervtkTemporalStatistics(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkTemporalStatistics>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
