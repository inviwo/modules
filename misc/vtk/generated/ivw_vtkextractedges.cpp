#include "ivw_vtkextractedges.h"
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
#include <vtkExtractEdges.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkExtractEdges> {
    static constexpr std::string_view uri = "vtk.vtkExtractEdges";
    static constexpr std::string_view className = "vtkExtractEdges";
    static constexpr std::string_view identifier = "ExtractEdges";
    static constexpr std::string_view displayName = "Extract Edges";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataSet", -1, R"(This property specifies the input to the Extract Edges
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;
    static constexpr std::string_view doc = R"(The Extract Edges
filter produces a wireframe version of the input dataset
by extracting all the edges of the dataset's cells as
lines. This filter operates on any type of data set and
produces polygonal output.)";
};

void registervtkExtractEdges(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkExtractEdges>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
