#include "ivw_vtkappenddatasets.h"
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
#include <vtkAppendDataSets.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkAppendDataSets& filter) {
        filter.SetOutputDataSetType(property.get());
        return true;
    }
    OptionPropertyInt property{
        "OutputDataSetType",
        "OutputDataSetType",
        R"(Determines the output type produced by this filter. Only input datasets compatible
with the output type will be merged in the output. For example, if the output type is
"Polygonal Mesh", then inputs of type "Image Data", "StructuredGrid", etc. will not be merged.
On the other hand, if the output type is "Unstructured Grid", then inputs of almost any type
will be merged in the output. Defaults to "Unstructured Grid".)"_help,
        {{"Polygonal Mesh", "Polygonal Mesh", 0}, {"Unstructured Grid", "Unstructured Grid", 4}},
        1};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkAppendDataSets> {
    static constexpr std::string_view uri = "vtk.vtkAppendDataSets";
    static constexpr std::string_view className = "vtkAppendDataSets";
    static constexpr std::string_view identifier = "Append";
    static constexpr std::string_view displayName = "Append Datasets";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataSet", -1, R"(This property specifies the datasets to be merged into a
single dataset by the Append Datasets filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;

    static constexpr std::string_view doc = R"(The Append
Datasets filter operates on multiple data sets of any type
(polygonal, structured, etc.). It merges their meshes
into a single dataset. If all inputs are polydata, the output
is a polydata, otherwise it is an unstructured grid.
Only the point and cell attributes that all of the input
datasets have in common will appear in the output.)";
};

void registervtkAppendDataSets(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkAppendDataSets>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
