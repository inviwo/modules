#include "ivw_vtkquadraturepointinterpolator.h"

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
#include <vtkQuadraturePointInterpolator.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkQuadraturePointInterpolator& filter) {
        if (property.size() == 0) return false;
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
        0};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{"QuadratureSchemeDefinition", "Quadrature Scheme Def",
                              R"(Specifies the offset array from which we interpolate
values to quadrature points.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkQuadraturePointInterpolator> {
    static constexpr std::string_view className = "vtkQuadraturePointInterpolator";
    static constexpr std::string_view identifier = "QuadraturePointInterpolator";
    static constexpr std::string_view displayName = "Interpolate to Quadrature Points";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkUnstructuredGrid", -1, R"(This property specifies the input of the filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc =
        R"("Create scalar/vector data arrays interpolated to quadrature
points.")";
};

void registervtkQuadraturePointInterpolator(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkQuadraturePointInterpolator>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
