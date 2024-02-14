#include "ivw_vtktensorglyph.h"
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
#include <vtkTensorGlyph.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkTensorGlyph& filter) {
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
        CompositeProperty tmp{
            "SelectInputTensors", "Tensors",
            R"(This property indicates the name of the tensor array on which to operate. The indicated array's
eigenvalues and eigenvectors are used for scaling and orienting the glyphs.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkTensorGlyph& filter) {
        filter.SetExtractEigenvalues(property.get());
        return true;
    }
    BoolProperty property{
        "ExtractEigenvalues", "ExtractEigenvalues",
        R"(Toggle whether to extract eigenvalues from tensor. If false, eigenvalues/eigenvectors are not extracted and
the columns of the tensor are taken as the eigenvectors (the norm of column, always positive, is the eigenvalue).
If true, the glyph is scaled and oriented according to eigenvalues and eigenvectors; additionally, eigenvalues
are provided as new data array.)"_help,
        true};
};

struct Wrapper2 {
    bool set(vtkTensorGlyph& filter) {
        filter.SetColorGlyphs(property.get());
        return true;
    }
    BoolProperty property{"ColorGlyphs", "ColorGlyphs",
                          R"(This property determines whether or not to color the glyphs.)"_help,
                          true};
};

struct Wrapper3 : FieldSelection {
    bool set(vtkTensorGlyph& filter) {
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
        CompositeProperty tmp{
            "SelectInputScalars", "Scalars",
            R"(This property indicates the name of the scalar array to use for coloring)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper4 {
    bool set(vtkTensorGlyph& filter) {
        filter.SetColorMode(property.get());
        return true;
    }
    OptionPropertyInt property{
        "ColorMode",
        "Color by",
        R"(This property determines whether input scalars or computed eigenvalues at the point should be used
to color the glyphs. If ThreeGlyphs is set and the eigenvalues are chosen for coloring then each glyph
is colored by the corresponding eigenvalue and if not set the color corresponding to the largest
eigenvalue is chosen.)"_help,
        {{"input scalars", "input scalars", 0}, {"eigenvalues", "eigenvalues", 1}},
        0};
};

struct Wrapper5 {
    bool set(vtkTensorGlyph& filter) {
        filter.SetScaleFactor(property.get());
        return true;
    }
    DoubleProperty property{
        "ScaleFactor",
        "ScaleFactor",
        R"(This property specifies the scale factor to scale every glyph by.)"_help,
        1.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(vtkTensorGlyph& filter) {
        filter.SetClampScaling(property.get());
        return true;
    }
    BoolProperty property{
        "LimitScalingByEigenvalues", "LimitScalingByEigenvalues",
        R"(This property determines whether scaling of glyphs by ScaleFactor times eigenvalue should be limited.
This is useful to prevent uncontrolled scaling near singularities.)"_help,
        false};
};

struct Wrapper7 {
    bool set(vtkTensorGlyph& filter) {
        filter.SetMaxScaleFactor(property.get());
        return true;
    }
    DoubleProperty property{
        "MaxScaleFactor",
        "MaxScaleFactor",
        R"(If scaling by eigenvalues should be limited, this value sets an upper limit for scale factor times
eigenvalue.)"_help,
        10.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(vtkTensorGlyph& filter) {
        filter.SetSymmetric(property.get());
        return true;
    }
    BoolProperty property{
        "Symmetric", "Symmetric",
        R"(This property determines whether or not to draw a mirror of each glyph.)"_help, false};
};

struct Wrapper9 {
    bool set(vtkTensorGlyph& filter) {
        filter.SetThreeGlyphs(property.get());
        return true;
    }
    BoolProperty property{
        "ThreeGlyphs", "ThreeGlyphs",
        R"(Toggle whether to produce three glyphs, each of which oriented along an eigenvector and scaled according
to the corresponding eigenvector.)"_help,
        false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkTensorGlyph> {
    static constexpr std::string_view uri = "vtk.vtkTensorGlyph";
    static constexpr std::string_view className = "vtkTensorGlyph";
    static constexpr std::string_view identifier = "TensorGlyph";
    static constexpr std::string_view displayName = "Tensor Glyph";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Input", "vtkDataSet", 6,
                  R"(This property specifies the input to the Glyph filter.)"},
        InputData{
            "Source", "vtkPolyData", -1,
            R"(This property determines which type of glyph will be placed at the points in the input dataset.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
    static constexpr std::string_view doc =
        R"(The Tensor Glyph filter generates an ellipsoid, cuboid, cylinder or superquadric glyph at every point in
the input data set. The glyphs are oriented and scaled according to eigenvalues and eigenvectors of tensor
point data of the input data set. The Tensor Glyph filter operates on any type of data set. Its output is
polygonal. This filter supports symmetric tensors. Symmetric tensor components are expected to have the
following order: XX, YY, ZZ, XY, YZ, XZ")";
};

void registervtkTensorGlyph(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkTensorGlyph>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
