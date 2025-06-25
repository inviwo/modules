#include "ivw_vtkimagetoamr.h"
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
#include <vtkImageToAMR.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkImageToAMR& filter) {
        filter.SetNumberOfLevels(property.get());
        return true;
    }
    IntProperty property{
        "NumberOfLevels",
        "Number of levels",
        R"(This property specifies the number of levels in the AMR data structure.)"_help,
        2,
        std::pair{1, ConstraintBehavior::Ignore},
        std::pair{20, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkImageToAMR& filter) {
        filter.SetMaximumNumberOfBlocks(property.get());
        return true;
    }
    IntProperty property{"MaximumNumberOfLevels",
                         "Maximum Number of Blocks",
                         R"(This property specifies the maximum number of blocks in the output
AMR data structure.)"_help,
                         100,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{100000, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkImageToAMR& filter) {
        filter.SetRefinementRatio(property.get());
        return true;
    }
    IntProperty property{"RefinementRatio",
                         "Refinement Ratio",
                         R"(This property specifies the refinement ratio between levels.)"_help,
                         2,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{16, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkImageToAMR> {
    static constexpr std::string_view uri = "vtk.vtkImageToAMR";
    static constexpr std::string_view className = "vtkImageToAMR";
    static constexpr std::string_view identifier = "ImageDataToAMR";
    static constexpr std::string_view displayName = "Image Data To AMR";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkImageData", -1, R"(This property specifies the input to the Cell Data to
Point Data filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2> properties;

    static constexpr std::string_view doc = R"ivw()ivw";
};

void registervtkImageToAMR(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkImageToAMR>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
