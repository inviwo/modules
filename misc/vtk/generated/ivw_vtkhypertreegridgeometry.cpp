#include "ivw_vtkhypertreegridgeometry.h"
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
#include <vtkHyperTreeGridGeometry.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkHyperTreeGridGeometry> {
    static constexpr std::string_view uri = "vtk.vtkHyperTreeGridGeometry";
    static constexpr std::string_view className = "vtkHyperTreeGridGeometry";
    static constexpr std::string_view identifier = "HyperTreeGridGeometry";
    static constexpr std::string_view displayName = "Hyper Tree Grid - Geometry Filter";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkHyperTreeGrid", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;

    static constexpr std::string_view doc =
        R"ivw(Generate PolyData representing the external surface of a HTG.)ivw";
};

void registervtkHyperTreeGridGeometry(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkHyperTreeGridGeometry>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
