#include "ivw_vtkcellcenters.h"

#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/ttk/processors/ttkgenericprocessor.h>
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
#include <vtkCellCenters.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkCellCenters& filter) {
        filter.SetVertexCells(property.get());
        return true;
    }
    BoolProperty property{"VertexCells", "VertexCells",
                          R"(If set to 1, a vertex cell will be generated per point
in the output. Otherwise only points will be generated.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkCellCenters> {
    static constexpr std::string_view className = "vtkCellCenters";
    static constexpr std::string_view identifier = "CellCenters";
    static constexpr std::string_view displayName = "Cell Centers";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataSet", -1, R"(This property specifies the input to the Cell Centers
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc = R"(The Cell Centers
filter places a point at the center of each cell in the
input data set. The center computed is the parametric
center of the cell, not necessarily the geometric or
bounding box center. The cell attributes of the input will
be associated with these newly created points of the
output. You have the option of creating a vertex cell per
point in the output. This is useful because vertex cells
are rendered, but points are not. The points themselves
could be used for placing glyphs (using the Glyph filter).
The Cell Centers filter takes any type of data set as
input and produces a polygonal data set as
output.)";
};

void registervtkCellCenters(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkCellCenters>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
