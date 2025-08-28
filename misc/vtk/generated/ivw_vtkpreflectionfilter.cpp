#include "ivw_vtkpreflectionfilter.h"
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
#include <vtkPReflectionFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkPReflectionFilter& filter) {
        filter.SetPlane(property.get());
        return true;
    }
    OptionPropertyInt property{"Plane",
                               "Plane",
                               R"(The value of this property determines which plane to
reflect across. If the value is X, Y, or Z, the value of the Center
property determines where the plane is placed along the specified axis.
The other six options (X Min, X Max, etc.) place the reflection plane
at the specified face of the bounding box of the input
dataset.)"_help,
                               {{"X Min", "X Min", 0},
                                {"Y Min", "Y Min", 1},
                                {"Z Min", "Z Min", 2},
                                {"X Max", "X Max", 3},
                                {"Y Max", "Y Max", 4},
                                {"Z Max", "Z Max", 5},
                                {"X", "X", 6},
                                {"Y", "Y", 7},
                                {"Z", "Z", 8}},
                               0};
};

struct Wrapper1 {
    bool set(vtkPReflectionFilter& filter) {
        filter.SetCenter(property.get());
        return true;
    }
    DoubleProperty property{"Center",
                            "Center",
                            R"(If the value of the Plane property is X, Y, or Z, then
the value of this property specifies the center of the reflection
plane.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkPReflectionFilter& filter) {
        filter.SetCopyInput(property.get());
        return true;
    }
    BoolProperty property{"CopyInput", "CopyInput",
                          R"(If this property is set to 1, the output will contain
the union of the input dataset and its reflection. Otherwise the output
will contain only the reflection of the input data.)"_help,
                          true};
};

struct Wrapper3 {
    bool set(vtkPReflectionFilter& filter) {
        filter.SetFlipAllInputArrays(property.get());
        return true;
    }
    BoolProperty property{"FlipAllInputArrays", "FlipAllInputArrays",
                          R"(If off, only Vectors, Normals and Tensors will be flipped.
If on, all 3-component data arrays ( considered as 3D vectors),
6-component data arrays (considered as symmetric tensors),
9-component data arrays (considered as tensors ) of signed type will be flipped.
All other arrays won't be flipped and will only be copied.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkPReflectionFilter> {
    static constexpr std::string_view uri = "vtk.vtkPReflectionFilter";
    static constexpr std::string_view className = "vtkPReflectionFilter";
    static constexpr std::string_view identifier = "ReflectionFilter";
    static constexpr std::string_view displayName = "Reflect";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkDataSet",
                  .numComp = -1,
                  .doc = R"(This property specifies the input to the Reflect
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;

    static constexpr std::string_view doc = R"ivw(The
Reflect filter reflects the input dataset across the
specified plane. This filter operates on any type of data
set and produces an unstructured grid
output.)ivw";
};

void registervtkPReflectionFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkPReflectionFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
