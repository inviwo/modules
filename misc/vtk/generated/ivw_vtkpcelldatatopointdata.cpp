#include "ivw_vtkpcelldatatopointdata.h"
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
#include <vtkPCellDataToPointData.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkPCellDataToPointData& filter) {
        filter.SetProcessAllArrays(property.get());
        return true;
    }
    BoolProperty property{"ProcessAllArrays", "ProcessAllArrays",
                          R"(The value of this property controls whether all input
cell arrays will be processed and converted into point arrays.)"_help,
                          true};
};

struct Wrapper1 {
    bool set(vtkPCellDataToPointData& filter) {
        filter.SetPassCellData(property.get());
        return true;
    }
    BoolProperty property{"PassCellData", "PassCellData",
                          R"(If this property is set to 1, then the input cell data
is passed through to the output; otherwise, only the generated point
data will be available in the output.)"_help,
                          false};
};

struct Wrapper2 {
    bool set(vtkPCellDataToPointData& filter) {
        filter.SetPieceInvariant(property.get());
        return true;
    }
    BoolProperty property{"PieceInvariant", "PieceInvariant",
                          R"(If the value of this property is set to 1, this filter
will request ghost levels so that the values at boundary points match
across processes. NOTE: Enabling this option might cause multiple
executions of the data source because more information is needed to
remove internal surfaces.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkPCellDataToPointData> {
    static constexpr std::string_view uri = "vtk.vtkPCellDataToPointData";
    static constexpr std::string_view className = "vtkPCellDataToPointData";
    static constexpr std::string_view identifier = "CellDataToPointData";
    static constexpr std::string_view displayName = "Cell Data to Point Data";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataSet", -1, R"(This property specifies the input to the Cell Data to
Point Data filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2> properties;

    static constexpr std::string_view doc = R"ivw(The Cell
Data to Point Data filter averages the values of the cell
attributes of the cells surrounding a point to compute
point attributes. The Cell Data to Point Data filter
operates on any type of data set, and the output data set
is of the same type as the input.)ivw";
};

void registervtkPCellDataToPointData(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkPCellDataToPointData>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
