#include "ivw_vtkcellderivatives.h"
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
#include <vtkCellDerivatives.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkCellDerivatives& filter) {
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
        CompositeProperty tmp{"SelectInputScalars", "Scalars",
                              R"(This property indicates the name of the scalar array to
differentiate.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 : FieldSelection {
    bool set(vtkCellDerivatives& filter) {
        if (name.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, fieldAssociation.get(), name.get().c_str());
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
        CompositeProperty tmp{"SelectInputVectors", "Vectors",
                              R"(This property indicates the name of the vector array to
differentiate.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(vtkCellDerivatives& filter) {
        filter.SetVectorMode(property.get());
        return true;
    }
    OptionPropertyInt property{"OutputVectorType",
                               "OutputVectorType",
                               R"(This property Controls how the filter works to generate
vector cell data. You can choose to compute the gradient of the input
scalars, or extract the vorticity of the computed vector gradient
tensor. By default, the filter will take the gradient of the input
scalar data.)"_help,
                               {{"Nothing", "Nothing", 0},
                                {"Scalar Gradient", "Scalar Gradient", 1},
                                {"Vorticity", "Vorticity", 2}},
                               1};
};

struct Wrapper3 {
    bool set(vtkCellDerivatives& filter) {
        filter.SetTensorMode(property.get());
        return true;
    }
    OptionPropertyInt property{"OutputTensorType",
                               "OutputTensorType",
                               R"(This property controls how the filter works to generate
tensor cell data. You can choose to compute the gradient of the input
vectors, or compute the strain tensor of the vector gradient tensor. By
default, the filter will take the gradient of the vector data to
construct a tensor.)"_help,
                               {{"Nothing", "Nothing", 0},
                                {"Vector Gradient", "Vector Gradient", 1},
                                {"Strain", "Strain", 2}},
                               1};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkCellDerivatives> {
    static constexpr std::string_view uri = "vtk.vtkCellDerivatives";
    static constexpr std::string_view className = "vtkCellDerivatives";
    static constexpr std::string_view identifier = "CellDerivatives";
    static constexpr std::string_view displayName = "Compute Derivatives";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", 1, R"(This property specifies the input to the
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;

    static constexpr std::string_view doc =
        R"ivw(CellDerivatives is a filter that computes derivatives of
scalars and vectors at the center of cells. You can choose
to generate different output including the scalar gradient
(a vector), computed tensor vorticity (a vector), gradient
of input vectors (a tensor), and strain matrix of the
input vectors (a tensor); or you may choose to pass data
through to the output.)ivw";
};

void registervtkCellDerivatives(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkCellDerivatives>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
