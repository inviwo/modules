#include "ivw_vtktemporaldatasetcache.h"

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
#include <vtkTemporalDataSetCache.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkTemporalDataSetCache& filter) {
        filter.SetCacheSize(property.get());
        return true;
    }
    IntProperty property{"CacheSize",
                         "CacheSize",
                         R"(The cache size determines the number of time steps that
can be cached at one time. The maximum number is 10. The minimum is 2
(since it makes little sense to cache less than that).)"_help,
                         2,
                         std::pair{2, ConstraintBehavior::Ignore},
                         std::pair{10, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkTemporalDataSetCache> {
    static constexpr std::string_view className = "vtkTemporalDataSetCache";
    static constexpr std::string_view identifier = "TemporalCache";
    static constexpr std::string_view displayName = "Temporal Cache";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataObject", -1, R"(This property specifies the input of the Temporal Cache
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc = R"(The Temporal Cache
can be used to save multiple copies of a data set at
different time steps to prevent thrashing in the pipeline
caused by downstream filters that adjust the requested
time step. For example, assume that there is a downstream
Temporal Interpolator filter. This filter will (usually)
request two time steps from the upstream filters, which in
turn (usually) causes the upstream filters to run twice,
once for each time step. The next time the interpolator
requests the same two time steps, they might force the
upstream filters to re-evaluate the same two time steps.
The Temporal Cache can keep copies of both of these time
steps and provide the requested data without having to run
upstream filters.)";
};

void registervtkTemporalDataSetCache(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkTemporalDataSetCache>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
