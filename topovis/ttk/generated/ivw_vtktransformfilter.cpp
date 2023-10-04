#include "ivw_vtktransformfilter.h"

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
#include <vtkTransformFilter.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkTransformFilter& filter) {
        filter.SetTransformAllInputVectors(property.get());
        return true;
    }
    BoolProperty property{"TransformAllInputVectors", "TransformAllInputVectors",
                          R"(If off, only Vectors and Normals will be transformed.
If on, all 3-component data arrays (considered as 3D vectors) will be transformed.
All other arrays won't be flipped and will only be copied.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkTransformFilter> {
    static constexpr std::string_view className = "vtkTransformFilter";
    static constexpr std::string_view identifier = "TransformFilter";
    static constexpr std::string_view displayName = "Transform";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkPointSet", -1, R"(This property specifies the input to the Transform
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc = R"(The Transform
filter allows you to specify the position, size, and
orientation of polygonal, unstructured grid, and
curvilinear data sets.)";
};

void registervtkTransformFilter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkTransformFilter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
