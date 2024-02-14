#include "ivw_vtkgenericgeometryfilter.h"
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
#include <vtkGenericGeometryFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkGenericGeometryFilter& filter) {
        filter.SetPassThroughCellIds(property.get());
        return true;
    }
    BoolProperty property{"PassThroughCellIds", "PassThroughCellIds",
                          R"(Select whether to forward original ids.)"_help, true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkGenericGeometryFilter> {
    static constexpr std::string_view uri = "vtk.vtkGenericGeometryFilter";
    static constexpr std::string_view className = "vtkGenericGeometryFilter";
    static constexpr std::string_view identifier = "GenericGeometryFilter";
    static constexpr std::string_view displayName = "Extract Generic Dataset Surface";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkGenericDataSet", -1, R"(Set the input to the Generic Geometry
Filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc = R"(Extract geometry from a higher-order
dataset.)";
};

void registervtkGenericGeometryFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkGenericGeometryFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
