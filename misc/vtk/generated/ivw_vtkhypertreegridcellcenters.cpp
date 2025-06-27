#include "ivw_vtkhypertreegridcellcenters.h"
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
#include <vtkHyperTreeGridCellCenters.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkHyperTreeGridCellCenters& filter) {
        filter.SetVertexCells(property.get());
        return true;
    }
    BoolProperty property{"CreateVertexCells", "CreateVertexCells",
                          R"(If enabled, vertex cells will be added to the output dataset. This
is useful for visualizing the output points, which are not rendered
otherwise.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkHyperTreeGridCellCenters> {
    static constexpr std::string_view uri = "vtk.vtkHyperTreeGridCellCenters";
    static constexpr std::string_view className = "vtkHyperTreeGridCellCenters";
    static constexpr std::string_view identifier = "HyperTreeGridCellCenters";
    static constexpr std::string_view displayName = "Hyper Tree Grid - Cell Centers";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkHyperTreeGrid", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;

    static constexpr std::string_view doc =
        R"ivw(This filter generates output points at the center of the leaf
cells in the hyper tree grid.
These points can be used for placing glyphs or labeling.
The cell attributes will be associated with the points in the output.)ivw";
};

void registervtkHyperTreeGridCellCenters(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkHyperTreeGridCellCenters>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
