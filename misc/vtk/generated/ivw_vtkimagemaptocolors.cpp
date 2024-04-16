#include "ivw_vtkimagemaptocolors.h"
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
#include <vtkImageMapToColors.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkImageMapToColors& filter) {
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
        CompositeProperty tmp{"SelectInputScalars", "Color By",
                              R"(This property specifies the name of the scalar array
from which we will color by.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkImageMapToColors& filter) {
        filter.SetNaNColor(property.get(0), property.get(1), property.get(2), property.get(3));
        return true;
    }
    IntVec4Property property{"NaNColor",
                             "RGBA NaN Color",
                             R"()"_help,
                             ivec4{0, 0, 0, 255},
                             std::pair{ivec4{0, 0, 0, 0}, ConstraintBehavior::Ignore},
                             std::pair{ivec4{255, 255, 255, 255}, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkImageMapToColors& filter) {
        filter.SetOutputFormat(property.get());
        return true;
    }
    OptionPropertyInt property{"OutputFormat",
                               "OutputFormat",
                               R"()"_help,
                               {{"Luminance", "Luminance", 1},
                                {"Luminance Alpha", "Luminance Alpha", 2},
                                {"RGB", "RGB", 3},
                                {"RGBA", "RGBA", 4}},
                               2};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkImageMapToColors> {
    static constexpr std::string_view uri = "vtk.vtkImageMapToColors";
    static constexpr std::string_view className = "vtkImageMapToColors";
    static constexpr std::string_view identifier = "ColorByArray";
    static constexpr std::string_view displayName = "Color By Array";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkImageData", 1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2> properties;

    static constexpr std::string_view doc =
        R"(This filter generate a color based image data based on a selected data scalar)";
};

void registervtkImageMapToColors(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkImageMapToColors>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
