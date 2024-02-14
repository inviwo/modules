#include "ivw_vtkhypertreegridaxisclip.h"
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
#include <vtkHyperTreeGridAxisClip.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkHyperTreeGridAxisClip& filter) {
        filter.SetClipType(property.get());
        return true;
    }
    OptionPropertyInt property{
        "ClipType",
        "ClipType",
        R"(Type of clip operation to use.)"_help,
        {{"Plane", "Plane", 0}, {"Box", "Box", 1}, {"Quadric", "Quadric", 2}},
        0};
};

struct Wrapper1 {
    bool set(vtkHyperTreeGridAxisClip& filter) {
        filter.SetPlaneNormalAxis(property.get());
        return true;
    }
    OptionPropertyInt property{
        "PlaneNormal",
        "PlaneNormal",
        R"(Axis to use as the normal to the clip plane.)"_help,
        {{"X Axis", "X Axis", 0}, {"Y Axis", "Y Axis", 1}, {"Z Axis", "Z Axis", 2}},
        0};
};

struct Wrapper2 {
    bool set(vtkHyperTreeGridAxisClip& filter) {
        filter.SetPlanePosition(property.get());
        return true;
    }
    DoubleProperty property{"PlanePosition",
                            "PlanePosition",
                            R"(Position of the clip plane along the normal axis.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkHyperTreeGridAxisClip& filter) {
        filter.SetInsideOut(property.get());
        return true;
    }
    BoolProperty property{"ClipBelowPlane", "ClipBelowPlane",
                          R"(By default, the portion of the dataset above the clip plane is kept.
Enabling this option will keep the portion below the plane.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkHyperTreeGridAxisClip> {
    static constexpr std::string_view uri = "vtk.vtkHyperTreeGridAxisClip";
    static constexpr std::string_view className = "vtkHyperTreeGridAxisClip";
    static constexpr std::string_view identifier = "HyperTreeGridAxisClip";
    static constexpr std::string_view displayName = "Hyper Tree Grid - Axis Clip";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkHyperTreeGrid", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;
    static constexpr std::string_view doc =
        R"(Clip an hyper tree grid along an axis aligned plane or box and output
a hyper tree grid with same dimensionality.)";
};

void registervtkHyperTreeGridAxisClip(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkHyperTreeGridAxisClip>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
