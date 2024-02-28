#include "ivw_vtkdistributeddatafilter.h"
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
#include <vtkDistributedDataFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkDistributedDataFilter& filter) {
        filter.SetBoundaryMode(property.get());
        return true;
    }
    OptionPropertyInt property{"BoundaryMode",
                               "BoundaryMode",
                               R"(This property determines how cells that lie on processor
boundaries are handled. The "Assign cells uniquely" option assigns each
boundary cell to exactly one process, which is useful for isosurfacing.
Selecting "Duplicate cells" causes the cells on the boundaries to be
copied to each process that shares that boundary. The "Divide cells"
option breaks cells across process boundary lines so that pieces of the
cell lie in different processes. This option is useful for volume
rendering.)"_help,
                               {{"Assign cells uniquely", "Assign cells uniquely", 0},
                                {"Duplicate cells", "Duplicate cells", 1},
                                {"Divide cells", "Divide cells", 2}},
                               0};
};

struct Wrapper1 {
    bool set(vtkDistributedDataFilter& filter) {
        filter.SetUseMinimalMemory(property.get());
        return true;
    }
    BoolProperty property{"UseMinimalMemory", "Minimal Memory",
                          R"(If this property is set to 1, the D3 filter requires
communication routines to use minimal memory than without this
restriction.)"_help,
                          false};
};

struct Wrapper2 {
    bool set(vtkDistributedDataFilter& filter) {
        filter.SetMinimumGhostLevel(property.get());
        return true;
    }
    IntProperty property{"MinimumGhostLevel",
                         "Minimum Number Of Ghost Levels",
                         R"(The minimum number of ghost levels to add to each
processor's output. If the pipeline also requests ghost levels, the
larger value will be used.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkDistributedDataFilter> {
    static constexpr std::string_view uri = "vtk.vtkDistributedDataFilter";
    static constexpr std::string_view className = "vtkDistributedDataFilter";
    static constexpr std::string_view identifier = "D3";
    static constexpr std::string_view displayName = "D3";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(This property specifies the input to the D3
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2> properties;
    static constexpr std::string_view doc = R"(The D3 filter is
available when ParaView is run in parallel. It operates on
any type of data set to evenly divide it across the
processors into spatially contiguous regions. The output
of this filter is of type unstructured
grid.)";
};

void registervtkDistributedDataFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkDistributedDataFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
