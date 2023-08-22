#include "ivw_vtkthreshold.h"

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
#include <vtkThreshold.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkThreshold& filter) {
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
        CompositeProperty tmp{"SelectInputScalars", "Scalars",
                              R"(The value of this property contains the name of the
scalar array from which to perform thresholding.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkThreshold& filter) {
        filter.SetLowerThreshold(property.get(0));
        filter.SetUpperThreshold(property.get(1));
        filter.SetThresholdFunction(vtkThreshold::THRESHOLD_BETWEEN);
        return true;
    }
    DoubleVec2Property property{"ThresholdBetween",
                                "Threshold Range",
                                R"(The values of this property specify the upper and lower
bounds of the thresholding operation.)"_help,
                                dvec2{0.0, 0.0},
                                std::pair{dvec2{0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec2{100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkThreshold& filter) {
        filter.SetAllScalars(property.get());
        return true;
    }
    BoolProperty property{"AllScalars", "AllScalars",
                          R"(If the value of this property is 1, then a cell is only
included in the output if the value of the selected array for all its
points is within the threshold. This is only relevant when thresholding
by a point-centered array.)"_help,
                          true};
};

struct Wrapper3 {
    bool set(vtkThreshold& filter) {
        filter.SetUseContinuousCellRange(property.get());
        return true;
    }
    BoolProperty property{"UseContinuousCellRange", "UseContinuousCellRange",
                          R"(If off, the vertex scalars are treated as a discrete set. If on, they
are treated as a continuous interval over the minimum and maximum. One
important "on" use case: When setting lower and upper threshold
equal to some value and turning AllScalars off, the results are
cells containing the isosurface for that value.  WARNING: Whether on
or off, for higher order input, the filter will not give accurate
results.)"_help,
                          false};
};

struct Wrapper4 {
    bool set(vtkThreshold& filter) {
        filter.SetInvert(property.get());
        return true;
    }
    BoolProperty property{
        "Invert", "Invert",
        R"(Invert the threshold results. That is, cells that would have been in the
output with this option off are excluded, while cells that would have been excluded from
the output are included.)"_help,
        false};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkThreshold> {
    static constexpr std::string_view className = "vtkThreshold";
    static constexpr std::string_view identifier = "Threshold";
    static constexpr std::string_view displayName = "Threshold";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", 1, R"(This property specifies the input to the Threshold
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;
    static constexpr std::string_view doc =
        R"(The Threshold filter extracts the portions of the input
dataset whose scalars lie within the specified range. This
filter operates on either point-centered or cell-centered
data. This filter operates on any type of dataset and
produces unstructured grid output. To select between these
two options, select either Point Data or Cell Data from
the Attribute Mode menu. Once the Attribute Mode has been
selected, choose the scalar array from which to threshold
the data from the Scalars menu. The Lower Threshold and
Upper Threshold sliders determine the range of the scalars
to retain in the output. The All Scalars check box only
takes effect when the Attribute Mode is set to Point Data.
If the All Scalars option is checked, then a cell will
only be passed to the output if the scalar values of all
of its points lie within the range indicated by the Lower
Threshold and Upper Threshold sliders. If unchecked, then
a cell will be added to the output if the specified scalar
value for any of its points is within the chosen
range.)";
};

void registervtkThreshold(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkThreshold>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
