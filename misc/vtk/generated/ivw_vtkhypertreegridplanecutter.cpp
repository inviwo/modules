#include "ivw_vtkhypertreegridplanecutter.h"
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
#include <vtkHyperTreeGridPlaneCutter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkHyperTreeGridPlaneCutter& filter) {
        filter.SetPlane(property.get(0), property.get(1), property.get(2), property.get(3));
        return true;
    }
    DoubleVec3Property property{
        "Plane",
        "Plane",
        R"(This property sets the parameters of the plane function.
Specify the plane with its [a,b,c,d] Cartesian coefficients a.x + b.y + c.z = d)"_help,
        dvec4{1.0, 0.0, 0.0, 0.0},
        std::pair{dvec4{0.0, 0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
        std::pair{dvec4{100.0, 100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkHyperTreeGridPlaneCutter& filter) {
        filter.SetDual(property.get());
        return true;
    }
    BoolProperty property{
        "UseDualGrid", "UseDualGrid",
        R"(If enabled, the output plane is computed using the dual grid, providing
perfect connectivity at the cost of speed. If disabled, the AMR mesh
is used directly, and the output is obtained faster and is more useful
for rendering.)"_help,
        false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkHyperTreeGridPlaneCutter> {
    static constexpr std::string_view uri = "vtk.vtkHyperTreeGridPlaneCutter";
    static constexpr std::string_view className = "vtkHyperTreeGridPlaneCutter";
    static constexpr std::string_view identifier = "HyperTreeGridSliceWithPlane";
    static constexpr std::string_view displayName = "Hyper Tree Grid - Slice With Plane";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkHyperTreeGrid", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;
    static constexpr std::string_view doc =
        R"(Takes as input an hyper tree grid and a single plane and generates the
polygonal data intersection surface.)";
};

void registervtkHyperTreeGridPlaneCutter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkHyperTreeGridPlaneCutter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
