#include "ivw_vtkgenericclip.h"

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
#include <vtkGenericClip.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkGenericClip& filter) {
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
                              R"(If clipping with scalars, this property specifies the
name of the scalar array on which to perform the clip
operation.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkGenericClip& filter) {
        filter.SetInsideOut(property.get());
        return true;
    }
    BoolProperty property{"InsideOut", "InsideOut",
                          R"(Choose which portion of the dataset should be clipped
away.)"_help,
                          false};
};

struct Wrapper2 {
    bool set(vtkGenericClip& filter) {
        filter.SetValue(property.get());
        return true;
    }
    DoubleProperty property{"Value",
                            "Value",
                            R"(If clipping with a scalar array, choose the clipping
value.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkGenericClip> {
    static constexpr std::string_view className = "vtkGenericClip";
    static constexpr std::string_view identifier = "GenericClip";
    static constexpr std::string_view displayName = "Clip Generic Dataset";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkGenericDataSet", -1, R"(Set the input to the Generic Clip
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2> properties;
    static constexpr std::string_view doc =
        R"(The Generic Clip filter cuts away a portion of the input
data set using a plane, a sphere, a box, or a scalar
value. The menu in the Clip Function portion of the
interface allows the user to select which implicit
function to use or whether to clip using a scalar value.
Making this selection loads the appropriate user
interface. For the implicit functions, the appropriate 3D
widget (plane, sphere, or box) is also displayed. The use
of these 3D widgets, including their user interface
components, is discussed in section 7.4. If an implicit
function is selected, the clip filter returns that portion
of the input data set that lies inside the function. If
Scalars is selected, then the user must specify a scalar
array to clip according to. The clip filter will return
the portions of the data set whose value in the selected
Scalars array is larger than the Clip value. Regardless of
the selection from the Clip Function menu, if the Inside
Out option is checked, the opposite portions of the data
set will be returned. This filter operates on all types of
data sets, and it returns unstructured grid data on
output.)";
};

void registervtkGenericClip(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkGenericClip>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
