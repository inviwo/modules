#include "ivw_vtkpointsettomoleculefilter.h"
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
#include <vtkPointSetToMoleculeFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkPointSetToMoleculeFilter& filter) {
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
        CompositeProperty tmp{"SelectInputScalars", "Atomic Numbers",
                              R"(This property indicates the name of the scalar array
corresponding to atomic numbers.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkPointSetToMoleculeFilter& filter) {
        filter.SetConvertLinesIntoBonds(property.get());
        return true;
    }
    BoolProperty property{
        "ConvertLinesIntoBonds", "ConvertLinesIntoBonds",
        R"(This property determines if the lines (cell of type VTK_LINE) are converted into bonds.)"_help,
        true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkPointSetToMoleculeFilter> {
    static constexpr std::string_view uri = "vtk.vtkPointSetToMoleculeFilter";
    static constexpr std::string_view className = "vtkPointSetToMoleculeFilter";
    static constexpr std::string_view identifier = "PointSetToMolecule";
    static constexpr std::string_view displayName = "Convert Into Molecule";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkPointSet", 1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;

    static constexpr std::string_view doc =
        R"ivw(Convert a point set into a molecule. Every point of the input becomes an atom
of the output molecule. It needs a point array containing the atomic numbers.)ivw";
};

void registervtkPointSetToMoleculeFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkPointSetToMoleculeFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
