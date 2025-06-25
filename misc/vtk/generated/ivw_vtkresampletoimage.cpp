#include "ivw_vtkresampletoimage.h"
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
#include <vtkResampleToImage.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkResampleToImage& filter) {
        filter.SetUseInputBounds(property.get());
        return true;
    }
    BoolProperty property{"UseInputBounds", "UseInputBounds",
                          R"(Use input bounds or custom ones?)"_help, true};
};

struct Wrapper1 {
    bool set(vtkResampleToImage& filter) {
        filter.SetSamplingDimensions(property.get(0), property.get(1), property.get(2));
        return true;
    }
    IntVec3Property property{"SamplingDimensions",
                             "SamplingDimensions",
                             R"(How many linear samples we want along each axis)"_help,
                             ivec3{100, 100, 100},
                             std::pair{ivec3{1, 1, 1}, ConstraintBehavior::Ignore},
                             std::pair{ivec3{100, 100, 100}, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkResampleToImage> {
    static constexpr std::string_view uri = "vtk.vtkResampleToImage";
    static constexpr std::string_view className = "vtkResampleToImage";
    static constexpr std::string_view identifier = "ResampleToImage";
    static constexpr std::string_view displayName = "Resample To Image";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(This property specifies the dataset whose data will
be probed)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;

    static constexpr std::string_view doc =
        R"ivw(Sample attributes using a 3D image as probing mesh.)ivw";
};

void registervtkResampleToImage(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkResampleToImage>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
