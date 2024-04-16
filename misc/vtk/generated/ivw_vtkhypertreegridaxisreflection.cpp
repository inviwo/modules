#include "ivw_vtkhypertreegridaxisreflection.h"
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
#include <vtkHyperTreeGridAxisReflection.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkHyperTreeGridAxisReflection& filter) {
        filter.SetPlane(property.get());
        return true;
    }
    OptionPropertyInt property{
        "PlaneNormal",
        "PlaneNormal",
        R"(Axis to use as the normal to the reflection plane.)"_help,
        {{"X Axis", "X Axis", 6}, {"Y Axis", "Y Axis", 7}, {"Z Axis", "Z Axis", 8}},
        0};
};

struct Wrapper1 {
    bool set(vtkHyperTreeGridAxisReflection& filter) {
        filter.SetCenter(property.get());
        return true;
    }
    DoubleProperty property{"PlanePosition",
                            "PlanePosition",
                            R"(Position of the reflection plane along the normal axis.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkHyperTreeGridAxisReflection> {
    static constexpr std::string_view uri = "vtk.vtkHyperTreeGridAxisReflection";
    static constexpr std::string_view className = "vtkHyperTreeGridAxisReflection";
    static constexpr std::string_view identifier = "HyperTreeGridAxisReflection";
    static constexpr std::string_view displayName = "Hyper Tree Grid - Axis Reflection";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkHyperTreeGrid", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;

    static constexpr std::string_view doc =
        R"(This filter reflect the cells of a hyper tree grid with respect to
one of the planes parallel to the bounding box of the data set.)";
};

void registervtkHyperTreeGridAxisReflection(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkHyperTreeGridAxisReflection>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
