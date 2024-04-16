#include "ivw_vtkpresamplefilter.h"
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
#include <vtkPResampleFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkPResampleFilter& filter) {
        filter.SetSamplingDimension(property.get(0), property.get(1), property.get(2));
        return true;
    }
    IntVec3Property property{"SamplingDimension",
                             "SamplingDimension",
                             R"(How many linear resampling we want along each axis)"_help,
                             ivec3{10, 10, 10},
                             std::pair{ivec3{2, 2, 2}, ConstraintBehavior::Ignore},
                             std::pair{ivec3{10000, 10000, 10000}, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkPResampleFilter& filter) {
        filter.SetUseInputBounds(property.get());
        return true;
    }
    BoolProperty property{"UseInputBounds", "UseInputBounds",
                          R"(Do we use input bounds or custom ones?)"_help, true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkPResampleFilter> {
    static constexpr std::string_view uri = "vtk.vtkPResampleFilter";
    static constexpr std::string_view className = "vtkPResampleFilter";
    static constexpr std::string_view identifier = "ImageResampling";
    static constexpr std::string_view displayName = "ImageResampling";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(This property specifies the dataset whose data will
be probed)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;

    static constexpr std::string_view doc =
        R"(Sample data attributes using a 3D image as probing mesh.)";
};

void registervtkPResampleFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkPResampleFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
