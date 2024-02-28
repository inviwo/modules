#include "ivw_vtkimagegradient.h"
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
#include <vtkImageGradient.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkImageGradient& filter) {
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
        CompositeProperty tmp{"SelectInputScalars", "SelectInputScalars",
                              R"(This property lists the name of the array from which to
compute the gradient.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkImageGradient& filter) {
        filter.SetDimensionality(property.get());
        return true;
    }
    OptionPropertyInt property{"Dimensionality",
                               "Dimensionality",
                               R"(This property indicates whether to compute the gradient
in two dimensions or in three. If the gradient is being computed in two
dimensions, the X and Y dimensions are used.)"_help,
                               {{"Two", "Two", 2}, {"Three", "Three", 3}},
                               1};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkImageGradient> {
    static constexpr std::string_view uri = "vtk.vtkImageGradient";
    static constexpr std::string_view className = "vtkImageGradient";
    static constexpr std::string_view identifier = "Gradient";
    static constexpr std::string_view displayName = "Gradient";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkImageData", 1, R"(This property specifies the input to the Gradient
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;
    static constexpr std::string_view doc = R"(The Gradient filter
computes the gradient vector at each point in an image or
volume. This filter uses central differences to compute
the gradients. The Gradient filter operates on uniform
rectilinear (image) data and produces image data
output.)";
};

void registervtkImageGradient(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkImageGradient>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
