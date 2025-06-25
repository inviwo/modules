#include "ivw_vtkquadraturepointsgenerator.h"
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
#include <vtkQuadraturePointsGenerator.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkQuadraturePointsGenerator& filter) {
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
        CompositeProperty tmp{"QuadratureSchemeDefinition", "Quadrature Scheme Def",
                              R"(Specifies the offset array from which we generate
quadrature points.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkQuadraturePointsGenerator> {
    static constexpr std::string_view uri = "vtk.vtkQuadraturePointsGenerator";
    static constexpr std::string_view className = "vtkQuadraturePointsGenerator";
    static constexpr std::string_view identifier = "QuadraturePointsGenerator";
    static constexpr std::string_view displayName = "Generate Quadrature Points";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkUnstructuredGrid", -1, R"(This property specifies the input of the filter.)"}};
    inline static std::array<OutputData, 1> outports = {
        OutputData{"outport0", "Quadrature Points", 0}};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;

    static constexpr std::string_view doc = R"ivw("Create a point set with data at quadrature
points.")ivw";
};

void registervtkQuadraturePointsGenerator(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkQuadraturePointsGenerator>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
