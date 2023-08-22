#include "ivw_vtkribbonfilter.h"

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
#include <vtkRibbonFilter.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkRibbonFilter& filter) {
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
                              R"(The value of this property indicates the name of the
input scalar array used by this filter. The width of the ribbons will
be varied based on the values in the specified array if the value of
the Width property is 1.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 : FieldSelection {
    bool set(vtkRibbonFilter& filter) {
        if (property.size() == 0) return false;
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
        0};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{"SelectInputVectors", "Vectors",
                              R"(The value of this property indicates the name of the
input vector array used by this filter. If the UseDefaultNormal
property is set to 0, the normal vectors for the ribbons come from the
specified vector array.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(vtkRibbonFilter& filter) {
        filter.SetWidth(property.get());
        return true;
    }
    DoubleProperty property{"Width",
                            "Width",
                            R"(If the VaryWidth property is set to 1, the value of this
property is the minimum ribbon width. If the VaryWidth property is set
to 0, the value of this property is half the width of the
ribbon.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkRibbonFilter& filter) {
        filter.SetAngle(property.get());
        return true;
    }
    DoubleProperty property{"Angle",
                            "Angle",
                            R"(The value of this property specifies the offset angle
(in degrees) of the ribbon from the line normal.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{360.0, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(vtkRibbonFilter& filter) {
        filter.SetUseDefaultNormal(property.get());
        return true;
    }
    BoolProperty property{"UseDefaultNormal", "UseDefaultNormal",
                          R"(If this property is set to 0, and the input contains no
vector array, then default ribbon normals will be generated
(DefaultNormal property); if a vector array has been set
(SelectInputVectors property), the ribbon normals will be set from the
specified array. If this property is set to 1, the default normal
(DefaultNormal property) will be used, regardless of whether the
SelectInputVectors property has been set.)"_help,
                          false};
};

struct Wrapper5 {
    bool set(vtkRibbonFilter& filter) {
        filter.SetDefaultNormal(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{"DefaultNormal",
                                "DefaultNormal",
                                R"(The value of this property specifies the normal to use
when the UseDefaultNormal property is set to 1 or the input contains no
vector array (SelectInputVectors property).)"_help,
                                dvec3{0.0, 0.0, 1.0},
                                std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(vtkRibbonFilter& filter) {
        filter.SetVaryWidth(property.get());
        return true;
    }
    BoolProperty property{"VaryWidth", "VaryWidth",
                          R"(If this property is set to 1, the ribbon width will be
scaled according to the scalar array specified in the
SelectInputScalars property. Toggle the variation of ribbon width with
scalar value.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkRibbonFilter> {
    static constexpr std::string_view className = "vtkRibbonFilter";
    static constexpr std::string_view identifier = "RibbonFilter";
    static constexpr std::string_view displayName = "Ribbon";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkPolyData", 3, R"(This property specifies the input to the Ribbon
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;
    static constexpr std::string_view doc = R"(The Ribbon
filter creates ribbons from the lines in the input data
set. This filter is useful for visualizing streamlines.
Both the input and output of this filter are polygonal
data. The input data set must also have at least one
point-centered vector array.)";
};

void registervtkRibbonFilter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkRibbonFilter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
