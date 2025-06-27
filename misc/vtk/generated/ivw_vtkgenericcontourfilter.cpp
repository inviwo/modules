#include "ivw_vtkgenericcontourfilter.h"
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
#include <vtkGenericContourFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkGenericContourFilter& filter) {
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
        CompositeProperty tmp{"SelectInputScalars", "Contour By",
                              R"(This property specifies the name of the scalar array
from which the contour filter will compute isolines and/or
isosurfaces.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkGenericContourFilter& filter) {
        filter.SetComputeNormals(property.get());
        return true;
    }
    BoolProperty property{"ComputeNormals", "ComputeNormals",
                          R"(Select whether to compute normals.)"_help, true};
};

struct Wrapper2 {
    bool set(vtkGenericContourFilter& filter) {
        filter.SetComputeGradients(property.get());
        return true;
    }
    BoolProperty property{"ComputeGradients", "ComputeGradients",
                          R"(Select whether to compute gradients.)"_help, false};
};

struct Wrapper3 {
    bool set(vtkGenericContourFilter& filter) {
        filter.SetComputeScalars(property.get());
        return true;
    }
    BoolProperty property{"ComputeScalars", "ComputeScalars",
                          R"(Select whether to compute scalars.)"_help, false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkGenericContourFilter> {
    static constexpr std::string_view uri = "vtk.vtkGenericContourFilter";
    static constexpr std::string_view className = "vtkGenericContourFilter";
    static constexpr std::string_view identifier = "GenericContour";
    static constexpr std::string_view displayName = "Contour Generic Dataset";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkGenericDataSet", 1, R"(Set the input to the Generic Contour
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {Group{"Isosurfaces", {"ContourValues"}},
                                                 Group{"Point Locator", {"Locator"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;

    static constexpr std::string_view doc = R"ivw(The Generic
Contour filter computes isolines or isosurfaces using a
selected point-centered scalar array. The available scalar
arrays are listed in the Scalars menu. The scalar range of
the selected array will be displayed. The interface for
adding contour values is very similar to the one for
selecting cut offsets (in the Cut filter). To add a single
contour value, select the value from the New Value slider
in the Add value portion of the interface and click the
Add button, or press Enter. To instead add several evenly
spaced contours, use the controls in the Generate range of
values section. Select the number of contour values to
generate using the Number of Values slider. The Range
slider controls the interval in which to generate the
contour values. Once the number of values and range have
been selected, click the Generate button. The new values
will be added to the Contour Values list. To delete a
value from the Contour Values list, select the value and
click the Delete button. (If no value is selected, the
last value in the list will be removed.) Clicking the
Delete All button removes all the values in the list. If
no values are in the Contour Values list when Accept is
pressed, the current value of the New Value slider will be
used. In addition to selecting contour values, you can
also select additional computations to perform. If any of
Compute Normals, Compute Gradients, or Compute Scalars is
selected, the appropriate computation will be performed,
and a corresponding point-centered array will be added to
the output. The Generic Contour filter operates on a
generic data set, but the input is required to have at
least one point-centered scalar (single-component) array.
The output of this filter is polygonal.)ivw";
};

void registervtkGenericContourFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkGenericContourFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
