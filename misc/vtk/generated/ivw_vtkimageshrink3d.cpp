#include "ivw_vtkimageshrink3d.h"
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
#include <vtkImageShrink3D.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkImageShrink3D& filter) {
        filter.SetShrinkFactors(property.get(0), property.get(1), property.get(2));
        return true;
    }
    IntVec3Property property{"ShrinkFactors",
                             "ShrinkFactors",
                             R"(The value of this property indicates the amount by which
to shrink along each axis.)"_help,
                             ivec3{1, 1, 1},
                             std::pair{ivec3{1, 1, 1}, ConstraintBehavior::Ignore},
                             std::pair{ivec3{100, 100, 100}, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkImageShrink3D& filter) {
        filter.SetAveraging(property.get());
        return true;
    }
    BoolProperty property{"Averaging", "Averaging",
                          R"(If the value of this property is 1, an average of
neighborhood scalar values will be used as the output scalar value for
each output point. If its value is 0, only subsampling will be
performed, and the original scalar values at the points will be
retained.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkImageShrink3D> {
    static constexpr std::string_view uri = "vtk.vtkImageShrink3D";
    static constexpr std::string_view className = "vtkImageShrink3D";
    static constexpr std::string_view identifier = "ImageShrink";
    static constexpr std::string_view displayName = "Image Shrink";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkImageData", -1, R"(This property specifies the input to the Image Shrink
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;

    static constexpr std::string_view doc = R"(The Image Shrink
filter reduces the size of an image/volume dataset by
subsampling it (i.e., extracting every nth pixel/voxel in
integer multiples). The subsampling rate can be set
separately for each dimension of the
image/volume.)";
};

void registervtkImageShrink3D(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkImageShrink3D>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
