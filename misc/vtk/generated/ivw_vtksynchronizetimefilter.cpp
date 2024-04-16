#include "ivw_vtksynchronizetimefilter.h"
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
#include <vtkSynchronizeTimeFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkSynchronizeTimeFilter& filter) {
        filter.SetRelativeTolerance(property.get());
        return true;
    }
    DoubleProperty property{"RelativeTolerance",
                            "RelativeTolerance",
                            R"()"_help,
                            1e-05,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkSynchronizeTimeFilter> {
    static constexpr std::string_view uri = "vtk.vtkSynchronizeTimeFilter";
    static constexpr std::string_view className = "vtkSynchronizeTimeFilter";
    static constexpr std::string_view identifier = "SynchronizeTime";
    static constexpr std::string_view displayName = "Synchronize Time";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"DataToPass", "vtkDataSet", -1,
                  R"(This property specifies the dataset whose geometry and
fields will be output.)"},
        InputData{"TimeToSync", "vtkDataSet", -1,
                  R"(This property specifies the dataset from which to obtain
the time step values.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;

    static constexpr std::string_view doc =
        R"(Synchronize time step values in the first input (Input) to time step
values in the second input (Source) that are considered close enough.
The outputted data set is from the first input and the number of
output time steps is also equal to the number of time steps in
the first input. Time step values in the first input that are
"close" to time step values in the second input are replaced
with the value from the second input. Close is determined to
be if the difference is less than RelativeTolerance multiplied
by the time range of the first input.)";
};

void registervtkSynchronizeTimeFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkSynchronizeTimeFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
