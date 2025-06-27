#include "ivw_vtkpointdatatocelldata.h"
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
#include <vtkPointDataToCellData.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkPointDataToCellData& filter) {
        filter.SetProcessAllArrays(property.get());
        return true;
    }
    BoolProperty property{"ProcessAllArrays", "ProcessAllArrays",
                          R"(The value of this property controls whether all input
point arrays will be processed and converted into cell arrays.)"_help,
                          true};
};

struct Wrapper1 {
    bool set(vtkPointDataToCellData& filter) {
        filter.SetPassPointData(property.get());
        return true;
    }
    BoolProperty property{"PassPointData", "PassPointData",
                          R"(The value of this property controls whether the input
point data will be passed to the output. If set to 1, then the input
point data is passed through to the output; otherwise, only generated
cell data is placed into the output.)"_help,
                          false};
};

struct Wrapper2 {
    bool set(vtkPointDataToCellData& filter) {
        filter.SetCategoricalData(property.get());
        return true;
    }
    BoolProperty property{"CategoricalData", "CategoricalData",
                          R"(Control whether the input point data is to be
treated as categorical. If  the data is categorical, then the
resultant cell data will be determined by  a majority rules
vote, with ties going to the smaller value.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkPointDataToCellData> {
    static constexpr std::string_view uri = "vtk.vtkPointDataToCellData";
    static constexpr std::string_view className = "vtkPointDataToCellData";
    static constexpr std::string_view identifier = "PointDataToCellData";
    static constexpr std::string_view displayName = "Point Data to Cell Data";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataSet", -1, R"(This property specifies the input to the Point Data to
Cell Data filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2> properties;

    static constexpr std::string_view doc = R"ivw(The Point
Data to Cell Data filter averages the values of the point
attributes of the points of a cell to compute cell
attributes. This filter operates on any type of dataset,
and the output dataset is the same type as the
input.)ivw";
};

void registervtkPointDataToCellData(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkPointDataToCellData>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
