#include "ivw_vtkrandomattributegenerator.h"
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
#include <vtkRandomAttributeGenerator.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkRandomAttributeGenerator& filter) {
        filter.SetDataType(property.get());
        return true;
    }
    OptionPropertyInt property{"DataType",
                               "DataType",
                               R"(Specify the type of array to create (all components of this
array are of this type). This holds true for all arrays that
are created.)"_help,
                               {{"Bit", "Bit", 1},
                                {"Char", "Char", 2},
                                {"UnsignedChar", "UnsignedChar", 3},
                                {"Short", "Short", 4},
                                {"UnsignedShort", "UnsignedShort", 5},
                                {"Int", "Int", 6},
                                {"UnsignedInt", "UnsignedInt", 7},
                                {"Long", "Long", 8},
                                {"UnsignedLong", "UnsignedLong", 9},
                                {"Float", "Float", 10},
                                {"Double", "Double", 11},
                                {"vtkIdType", "vtkIdType", 12}},
                               2};
};

struct Wrapper1 {
    bool set(vtkRandomAttributeGenerator& filter) {
        filter.SetComponentRange(property.get(0), property.get(1));
        return true;
    }
    DoubleVec2Property property{"ComponentRange",
                                "ComponentRange",
                                R"(Set the range values (minimum and maximum) for
each component. This applies to all data that is
generated.)"_help,
                                dvec2{0.0, 255.0},
                                std::pair{dvec2{0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec2{100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkRandomAttributeGenerator& filter) {
        filter.SetAttributesConstantPerBlock(property.get());
        return true;
    }
    BoolProperty property{"AttributesConstantPerBlock", "AttributesConstantPerBlock",
                          R"(Indicate that the generated attributes are
constant within a block. This can be used to highlight
blocks in a composite dataset.)"_help,
                          false};
};

struct Wrapper3 {
    bool set(vtkRandomAttributeGenerator& filter) {
        filter.SetGeneratePointScalars(property.get());
        return true;
    }
    BoolProperty property{"GeneratePointScalars", "GeneratePointScalars",
                          R"(Indicate that point scalars are to be
generated.)"_help,
                          false};
};

struct Wrapper4 {
    bool set(vtkRandomAttributeGenerator& filter) {
        filter.SetGeneratePointVectors(property.get());
        return true;
    }
    BoolProperty property{"GeneratePointVectors", "GeneratePointVectors",
                          R"(Indicate that point vectors are to be
generated.)"_help,
                          false};
};

struct Wrapper5 {
    bool set(vtkRandomAttributeGenerator& filter) {
        filter.SetGenerateCellScalars(property.get());
        return true;
    }
    BoolProperty property{"GenerateCellScalars", "GenerateCellScalars",
                          R"(Indicate that point scalars are to be
generated.)"_help,
                          false};
};

struct Wrapper6 {
    bool set(vtkRandomAttributeGenerator& filter) {
        filter.SetGenerateCellVectors(property.get());
        return true;
    }
    BoolProperty property{"GenerateCellVectors", "GenerateCellVectors",
                          R"(Indicate that point vectors are to be
generated.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkRandomAttributeGenerator> {
    static constexpr std::string_view uri = "vtk.vtkRandomAttributeGenerator";
    static constexpr std::string_view className = "vtkRandomAttributeGenerator";
    static constexpr std::string_view identifier = "RandomAttributeGenerator";
    static constexpr std::string_view displayName = "Random Attributes";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataSet", -1, R"(This property specifies the input to the Random Scalars
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;

    static constexpr std::string_view doc =
        R"ivw(The Random Attributes filter creates random attributes
including scalars and vectors. These attributes can be
generated as point data or cell data. The generation of each
component is normalized between a user-specified minimum and
maximum value.

This filter provides that capability to specify the data type
of the attributes and the range for each of the components.)ivw";
};

void registervtkRandomAttributeGenerator(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkRandomAttributeGenerator>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
