#include "ivw_vtkaggregatedatasetfilter.h"

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
#include <vtkAggregateDataSetFilter.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkAggregateDataSetFilter& filter) {
        filter.SetNumberOfTargetProcesses(property.get());
        return true;
    }
    IntProperty property{"NumberOfTargetProcesses",
                         "NumberOfTargetProcesses",
                         R"(This property specifies the number of target processes to
aggregate the dataset onto.)"_help,
                         1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkAggregateDataSetFilter> {
    static constexpr std::string_view className = "vtkAggregateDataSetFilter";
    static constexpr std::string_view identifier = "AggregateDataSet";
    static constexpr std::string_view displayName = "Aggregate Dataset";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataSet", -1, R"(This property specifies the input to the filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc =
        R"(This filter aggregates a dataset onto a subset of processes.)";
};

void registervtkAggregateDataSetFilter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkAggregateDataSetFilter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
