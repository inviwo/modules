#include "ivw_vtkhypertreegridaxiscut.h"

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
#include <vtkHyperTreeGridAxisCut.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkHyperTreeGridAxisCut& filter) {
        filter.SetPlaneNormalAxis(property.get());
        return true;
    }
    OptionPropertyInt property{
        "PlaneNormal",
        "PlaneNormal",
        R"(Axis to use as the normal to the cut plane.)"_help,
        {{"X Axis", "X Axis", 0}, {"Y Axis", "Y Axis", 1}, {"Z Axis", "Z Axis", 2}},
        0};
};

struct Wrapper1 {
    bool set(vtkHyperTreeGridAxisCut& filter) {
        filter.SetPlanePosition(property.get());
        return true;
    }
    DoubleProperty property{"PlanePosition",
                            "PlanePosition",
                            R"(Position of the cut plane along the normal axis.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkHyperTreeGridAxisCut> {
    static constexpr std::string_view className = "vtkHyperTreeGridAxisCut";
    static constexpr std::string_view identifier = "HyperTreeGridAxisCut";
    static constexpr std::string_view displayName = "Hyper Tree Grid - Axis Cut";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkHyperTreeGrid", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;
    static constexpr std::string_view doc =
        R"(Cut a hyper tree grid along an axis-aligned plane and output a new hyper
tree grid. Only works for 3D grids.)";
};

void registervtkHyperTreeGridAxisCut(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkHyperTreeGridAxisCut>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
