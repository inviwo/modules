#include "ivw_vtkimagedatatouniformgrid.h"
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
#include <vtkImageDataToUniformGrid.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkImageDataToUniformGrid& filter) {
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
                              R"(Specify the array to use for blanking.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkImageDataToUniformGrid& filter) {
        filter.SetReverse(property.get());
        return true;
    }
    BoolProperty property{
        "Reverse", "Reverse",
        R"(Reverse the array value to whether or not a point or cell is blanked.)"_help, false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkImageDataToUniformGrid> {
    static constexpr std::string_view uri = "vtk.vtkImageDataToUniformGrid";
    static constexpr std::string_view className = "vtkImageDataToUniformGrid";
    static constexpr std::string_view identifier = "ImageDataToUniformGrid";
    static constexpr std::string_view displayName = "Image Data To Uniform Grid";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input", .dataType = "vtkImageData", .numComp = 1, .doc = R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;

    static constexpr std::string_view doc =
        R"ivw(Create a vtkUniformGrid from a vtkImageData by passing in arrays to be used
for point and/or cell blanking. By default, values of 0 in the specified
array will result in a point or cell being blanked. Use Reverse to switch this.)ivw";
};

void registervtkImageDataToUniformGrid(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkImageDataToUniformGrid>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
