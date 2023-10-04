#include "ivw_vtkhypertreegriddepthlimiter.h"

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
#include <vtkHyperTreeGridDepthLimiter.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkHyperTreeGridDepthLimiter& filter) {
        filter.SetDepth(property.get());
        return true;
    }
    IntProperty property{"SetDepth",
                         "SetDepth",
                         R"(Maximum depth to which the output grid should be limited.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkHyperTreeGridDepthLimiter> {
    static constexpr std::string_view className = "vtkHyperTreeGridDepthLimiter";
    static constexpr std::string_view identifier = "HyperTreeGridDepthLimiter";
    static constexpr std::string_view displayName = "Hyper Tree Grid - Depth Limiter";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkHyperTreeGrid", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc =
        R"(Extract all levels down to a specified depth from a hyper tree grid.
If the required depth is greater or equal to the maximum level of the
input grid, then the output is identical.
Note that when a material mask is present, the geometry extent of the
output grid is guaranteed to contain that of the input tree, but the
former might be strictly larger than the latter. This is not a bug
but an expected behavior of which the user should be aware.)";
};

void registervtkHyperTreeGridDepthLimiter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkHyperTreeGridDepthLimiter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
