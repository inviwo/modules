#include "ivw_vtktessellatorfilter.h"
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
#include <vtkTessellatorFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkTessellatorFilter& filter) {
        filter.SetOutputDimension(property.get());
        return true;
    }
    IntProperty property{"OutputDimension",
                         "OutputDimension",
                         R"(The value of this property sets the maximum
dimensionality of the output tessellation. When the value of this
property is 3, 3D cells produce tetrahedra, 2D cells produce triangles,
and 1D cells produce line segments. When the value is 2, 3D cells will
have their boundaries tessellated with triangles. When the value is 1,
all cells except points produce line segments.)"_help,
                         3,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{3, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkTessellatorFilter& filter) {
        filter.SetChordError(property.get());
        return true;
    }
    DoubleProperty property{"ChordError",
                            "ChordError",
                            R"(This property controls the maximum chord error allowed
at any edge midpoint in the output tessellation. The chord error is
measured as the distance between the midpoint of any output edge and
the original nonlinear geometry.)"_help,
                            0.001,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkTessellatorFilter& filter) {
        filter.SetMaximumNumberOfSubdivisions(property.get());
        return true;
    }
    IntProperty property{"MaximumNumberOfSubdivisions",
                         "Maximum Number of Subdivisions",
                         R"(This property specifies the maximum number of times an
edge may be subdivided. Increasing this number allows further
refinement but can drastically increase the computational and storage
requirements, especially when the value of the OutputDimension property
is 3.)"_help,
                         3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{8, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkTessellatorFilter& filter) {
        filter.SetMergePoints(property.get());
        return true;
    }
    BoolProperty property{"MergePoints", "MergePoints",
                          R"(If the value of this property is set to 1, coincident
vertices will be merged after tessellation has occurred. Only geometry
is considered during the merge and the first vertex encountered is the
one whose point attributes will be used. Any discontinuities in point
fields will be lost. On the other hand, many operations, such as
streamline generation, require coincident vertices to be merged. Toggle
whether to merge coincident vertices.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkTessellatorFilter> {
    static constexpr std::string_view uri = "vtk.vtkTessellatorFilter";
    static constexpr std::string_view className = "vtkTessellatorFilter";
    static constexpr std::string_view identifier = "TessellatorFilter";
    static constexpr std::string_view displayName = "Tessellate";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkPolyData",
                  .numComp = -1,
                  .doc = R"(This property specifies the input to the Tessellate
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;

    static constexpr std::string_view doc = R"ivw(The Tessellate filter
tessellates cells with nonlinear geometry and/or scalar
fields into a simplicial complex with linearly
interpolated field values that more closely approximate
the original field. This is useful for datasets containing
quadratic cells.)ivw";
};

void registervtkTessellatorFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkTessellatorFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
