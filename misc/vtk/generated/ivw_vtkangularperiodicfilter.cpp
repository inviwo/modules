#include "ivw_vtkangularperiodicfilter.h"
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
#include <vtkAngularPeriodicFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkAngularPeriodicFilter& filter) {
        filter.SetIterationMode(property.get());
        return true;
    }
    OptionPropertyInt property{
        "IterationMode",
        "IterationMode",
        R"(This property specifies the mode of iteration, either a user-provided number
of periods, or the maximum number of periods to rotate to 360 degrees.)"_help,
        {{"Manual", "Manual", 0}, {"Maximum", "Maximum", 1}},
        1};
};

struct Wrapper1 {
    bool set(vtkAngularPeriodicFilter& filter) {
        filter.SetNumberOfPeriods(property.get());
        return true;
    }
    IntProperty property{"NumberOfPeriods",
                         "NumberOfPeriods",
                         R"(This property specifies the number of iteration)"_help,
                         3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkAngularPeriodicFilter& filter) {
        filter.SetRotationMode(property.get());
        return true;
    }
    OptionPropertyInt property{
        "RotationMode",
        "RotationMode",
        R"(This property specifies the mode of rotation, either from a user provided
angle or from an array in the data.)"_help,
        {{"Direct Angle", "Direct Angle", 0}, {"Array Value", "Array Value", 1}},
        0};
};

struct Wrapper3 {
    bool set(vtkAngularPeriodicFilter& filter) {
        filter.SetRotationAngle(property.get());
        return true;
    }
    DoubleProperty property{"RotationAngle",
                            "RotationAngle",
                            R"(Rotation angle in degree.)"_help,
                            10.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(vtkAngularPeriodicFilter& filter) {
        filter.SetRotationArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"RotationArrayName", "RotationArrayName",
                            R"(Field array name that contains the rotation angle in radians.)"_help,
                            "periodic angle"};
};

struct Wrapper5 {
    bool set(vtkAngularPeriodicFilter& filter) {
        filter.SetRotationAxis(property.get());
        return true;
    }
    OptionPropertyInt property{
        "Axis",
        "Axis",
        R"(This property specifies the axis of rotation)"_help,
        {{"Axis X", "Axis X", 0}, {"Axis Y", "Axis Y", 1}, {"Axis Z", "Axis Z", 2}},
        0};
};

struct Wrapper6 {
    bool set(vtkAngularPeriodicFilter& filter) {
        filter.SetCenter(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{"Center",
                                "Center",
                                R"(This property specifies the 3D coordinates for the
center of the rotation.)"_help,
                                dvec3{0.0, 0.0, 0.0},
                                std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(vtkAngularPeriodicFilter& filter) {
        filter.SetComputeRotationsOnTheFly(property.get());
        return true;
    }
    BoolProperty property{"ComputeRotationsOnTheFly", "ComputeRotationsOnTheFly",
                          R"(Specify whether the rotations should be computed on-the-fly, which is
compute intensive, or if the arrays should be explicitly generated and
stored, at the cost of using more memory.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkAngularPeriodicFilter> {
    static constexpr std::string_view uri = "vtk.vtkAngularPeriodicFilter";
    static constexpr std::string_view className = "vtkAngularPeriodicFilter";
    static constexpr std::string_view identifier = "AngularPeriodicFilter";
    static constexpr std::string_view displayName = "Angular Periodic Filter";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkDataSet",
                  .numComp = -1,
                  .doc = R"(This property specifies the input to the Periodic filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;

    static constexpr std::string_view doc = R"ivw(This filter generate a periodic
multiblock dataset)ivw";
};

void registervtkAngularPeriodicFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkAngularPeriodicFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
