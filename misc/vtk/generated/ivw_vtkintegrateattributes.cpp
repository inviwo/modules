#include "ivw_vtkintegrateattributes.h"
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
#include <vtkIntegrateAttributes.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkIntegrateAttributes& filter) {
        filter.SetDivideAllCellDataByVolume(property.get());
        return true;
    }
    BoolProperty property{"DivideCellDataByVolume", "DivideCellDataByVolume",
                          R"(This property specifies if the output data will be divided by the
volume/area computed for the integrated cells.  If it is on, then each
value in the output cell data will be divided by the area/volume.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkIntegrateAttributes> {
    static constexpr std::string_view uri = "vtk.vtkIntegrateAttributes";
    static constexpr std::string_view className = "vtkIntegrateAttributes";
    static constexpr std::string_view identifier = "IntegrateAttributes";
    static constexpr std::string_view displayName = "Integrate Variables";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(This property specifies the input to the Integrate
Attributes filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;

    static constexpr std::string_view doc =
        R"ivw(The Integrate Attributes filter integrates point and cell
data over lines and surfaces. It also computes length of
lines, area of surface, or volume.)ivw";
};

void registervtkIntegrateAttributes(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkIntegrateAttributes>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
