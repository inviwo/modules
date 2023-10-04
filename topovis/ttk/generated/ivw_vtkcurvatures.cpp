#include "ivw_vtkcurvatures.h"

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
#include <vtkCurvatures.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkCurvatures& filter) {
        filter.SetInvertMeanCurvature(property.get());
        return true;
    }
    BoolProperty property{"InvertMeanCurvature", "InvertMeanCurvature",
                          R"(If this property is set to 1, the mean curvature
calculation will be inverted. This is useful for meshes with
inward-pointing normals.)"_help,
                          false};
};

struct Wrapper1 {
    bool set(vtkCurvatures& filter) {
        filter.SetCurvatureType(property.get());
        return true;
    }
    OptionPropertyInt property{"CurvatureType",
                               "CurvatureType",
                               R"(This property specifies which type of curvature to
compute.)"_help,
                               {{"Gaussian", "Gaussian", 0}, {"Mean", "Mean", 1}},
                               0};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkCurvatures> {
    static constexpr std::string_view className = "vtkCurvatures";
    static constexpr std::string_view identifier = "Curvatures";
    static constexpr std::string_view displayName = "Curvature";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkPolyData", -1, R"(This property specifies the input to the Curvature
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;
    static constexpr std::string_view doc = R"(The
Curvature filter computes the curvature at each point in a
polygonal data set. This filter supports both Gaussian and
mean curvatures. ; the type can be selected from the
Curvature type menu button.)";
};

void registervtkCurvatures(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkCurvatures>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
