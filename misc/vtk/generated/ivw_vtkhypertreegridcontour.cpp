#include "ivw_vtkhypertreegridcontour.h"
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
#include <vtkHyperTreeGridContour.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkHyperTreeGridContour& filter) {
        if (name.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, fieldAssociation.get(), name.get().c_str());
        return true;
    }
    OptionPropertyString name{"name", "Name", {}, 0};

    OptionProperty<vtkDataObject::FieldAssociations> fieldAssociation{
        "fieldAssociation",
        "Field Association",
        {{"points", "Points", vtkDataObject::FIELD_ASSOCIATION_POINTS},
         {"cells", "Cells", vtkDataObject::FIELD_ASSOCIATION_CELLS},
         {"none", "None", vtkDataObject::FIELD_ASSOCIATION_NONE},
         {"pointsThenCells", "Points then Cells",
          vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS}},
        3};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{"SelectInputScalars", "Contour By",
                              R"(This property specifies the name of the scalar array
from which the contour filter will compute isolines and/or
isosurfaces.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkHyperTreeGridContour> {
    static constexpr std::string_view uri = "vtk.vtkHyperTreeGridContour";
    static constexpr std::string_view className = "vtkHyperTreeGridContour";
    static constexpr std::string_view identifier = "HyperTreeGridContour";
    static constexpr std::string_view displayName = "Hyper Tree Grid - Contour";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkHyperTreeGrid", -1, R"(This property specifies the input to the converter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;

    static constexpr std::string_view doc =
        R"ivw(This filter extracts isocontours directly from HyperTreeGrid input datasets.)ivw";
};

void registervtkHyperTreeGridContour(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkHyperTreeGridContour>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
