#include "ivw_vtktubefilter.h"
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
#include <vtkTubeFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkTubeFilter& filter) {
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
                              R"(This property indicates the name of the scalar array on
which to operate. The indicated array may be used for scaling the
tubes. (See the VaryRadius property.))"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 : FieldSelection {
    bool set(vtkTubeFilter& filter) {
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
                              R"(This property indicates the name of the vector array on
which to operate. The indicated array may be used for scaling and/or
orienting the tubes. (See the VaryRadius property.))"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(vtkTubeFilter& filter) {
        filter.SetNumberOfSides(property.get());
        return true;
    }
    IntProperty property{"NumberOfSides",
                         "Number of Sides",
                         R"(The value of this property indicates the number of faces
around the circumference of the tube.)"_help,
                         6,
                         std::pair{3, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkTubeFilter& filter) {
        filter.SetCapping(property.get());
        return true;
    }
    BoolProperty property{"Capping", "Capping",
                          R"(If this property is set to 1, endcaps will be drawn on
the tube. Otherwise the ends of the tube will be open.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(vtkTubeFilter& filter) {
        filter.SetRadius(property.get());
        return true;
    }
    DoubleProperty property{"Radius",
                            "Radius",
                            R"(The value of this property sets the radius of the tube.
If the radius is varying (VaryRadius property), then this value is the
minimum radius.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(vtkTubeFilter& filter) {
        filter.SetVaryRadius(property.get());
        return true;
    }
    OptionPropertyInt property{"VaryRadius",
                               "VaryRadius",
                               R"(The property determines whether/how to vary the radius
of the tube. If varying by scalar (1), the tube radius is based on the
point-based scalar values in the dataset. If it is varied by vector,
the vector magnitude is used in varying the radius.)"_help,
                               {{"Off", "Off", 0},
                                {"By Scalar", "By Scalar", 1},
                                {"By Vector", "By Vector", 2},
                                {"By Absolute Scalar", "By Absolute Scalar", 3}},
                               0};
};

struct Wrapper6 {
    bool set(vtkTubeFilter& filter) {
        filter.SetRadiusFactor(property.get());
        return true;
    }
    DoubleProperty property{"RadiusFactor",
                            "RadiusFactor",
                            R"(If varying the radius (VaryRadius property), the
property sets the maximum tube radius in terms of a multiple of the
minimum radius. If not varying the radius, this value has no
effect.)"_help,
                            10.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(vtkTubeFilter& filter) {
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

struct Wrapper8 {
    bool set(vtkTubeFilter& filter) {
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

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkTubeFilter> {
    static constexpr std::string_view uri = "vtk.vtkTubeFilter";
    static constexpr std::string_view className = "vtkTubeFilter";
    static constexpr std::string_view identifier = "TubeFilter";
    static constexpr std::string_view displayName = "Tube";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkPolyData", 1, R"(This property specifies the input to the Tube
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;

    static constexpr std::string_view doc = R"(The Tube filter
creates tubes around the lines in the input polygonal
dataset. The output is also polygonal.)";
};

void registervtkTubeFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkTubeFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
