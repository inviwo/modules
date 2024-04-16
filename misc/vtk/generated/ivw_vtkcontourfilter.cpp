#include "ivw_vtkcontourfilter.h"
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
#include <vtkContourFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkContourFilter& filter) {
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
    bool set(vtkContourFilter& filter) {
        filter.SetComputeNormals(property.get());
        return true;
    }
    BoolProperty property{"ComputeNormals", "ComputeNormals",
                          R"(If this property is set to 1, a scalar array containing
a normal value at each point in the isosurface or isoline will be
created by the contour filter; otherwise an array of normals will not
be computed. This operation is fairly expensive both in terms of
computation time and memory required, so if the output dataset produced
by the contour filter will be processed by filters that modify the
dataset's topology or geometry, it may be wise to set the value of this
property to 0. Select whether to compute normals.)"_help,
                          true};
};

struct Wrapper2 {
    bool set(vtkContourFilter& filter) {
        filter.SetComputeGradients(property.get());
        return true;
    }
    BoolProperty property{"ComputeGradients", "ComputeGradients",
                          R"(If this property is set to 1, a scalar array containing
a gradient value at each point in the isosurface or isoline will be
created by this filter; otherwise an array of gradients will not be
computed. This operation is fairly expensive both in terms of
computation time and memory required, so if the output dataset produced
by the contour filter will be processed by filters that modify the
dataset's topology or geometry, it may be wise to set the value of this
property to 0. Not that if ComputeNormals is set to 1, then gradients
will have to be calculated, but they will only be stored in the output
dataset if ComputeGradients is also set to 1.)"_help,
                          false};
};

struct Wrapper3 {
    bool set(vtkContourFilter& filter) {
        filter.SetComputeScalars(property.get());
        return true;
    }
    BoolProperty property{"ComputeScalars", "ComputeScalars",
                          R"(If this property is set to 1, an array of scalars
(containing the contour value) will be added to the output dataset. If
set to 0, the output will not contain this array.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(vtkContourFilter& filter) {
        filter.SetOutputPointsPrecision(property.get());
        return true;
    }
    OptionPropertyInt property{
        "OutputPointsPrecision",
        "OutputPointsPrecision",
        R"(Select the output precision of the coordinates. **Single** sets the
output to single-precision floating-point (i.e., float), **Double**
sets it to double-precision floating-point (i.e., double), and
**Default** sets it to the same precision as the precision of the
points in the input. Defaults to ***Single***.)"_help,
        {{"Single", "Single", 0}, {"Double", "Double", 1}, {"Same as input", "Same as input", 2}},
        2};
};

struct Wrapper5 {
    bool set(vtkContourFilter& filter) {
        filter.SetGenerateTriangles(property.get());
        return true;
    }
    BoolProperty property{"GenerateTriangles", "GenerateTriangles",
                          R"(This parameter controls whether to produce triangles in the output.
Warning: Many filters do not properly handle non-triangular polygons.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkContourFilter> {
    static constexpr std::string_view uri = "vtk.vtkContourFilter";
    static constexpr std::string_view className = "vtkContourFilter";
    static constexpr std::string_view identifier = "Contour";
    static constexpr std::string_view displayName = "Contour";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataSet", 1, R"(This property specifies the input dataset to be used by
the contour filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {Group{"Isosurfaces", {"ContourValues"}},
                                                 Group{"Point Locator", {"Locator"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;

    static constexpr std::string_view doc = R"(The Contour
filter computes isolines or isosurfaces using a selected
point-centered scalar array. The Contour filter operates
on any type of data set, but the input is required to have
at least one point-centered scalar (single-component)
array. The output of this filter is
polygonal.)";
};

void registervtkContourFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkContourFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
