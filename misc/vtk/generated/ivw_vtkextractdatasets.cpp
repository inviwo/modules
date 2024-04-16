#include "ivw_vtkextractdatasets.h"
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
#include <vtkExtractDataSets.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkExtractDataSets> {
    static constexpr std::string_view uri = "vtk.vtkExtractDataSets";
    static constexpr std::string_view className = "vtkExtractDataSets";
    static constexpr std::string_view identifier = "ExtractHierarchicalDataSets";
    static constexpr std::string_view displayName = "Extract AMR Blocks";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkUniformGridAMR", -1, R"(This property specifies the input to the Extract
Datasets filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;

    static constexpr std::string_view doc = R"(This filter extracts a list
of datasets from hierarchical datasets.)";
};

void registervtkExtractDataSets(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkExtractDataSets>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
