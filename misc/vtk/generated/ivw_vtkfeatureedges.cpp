#include "ivw_vtkfeatureedges.h"
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
#include <vtkFeatureEdges.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkFeatureEdges& filter) {
        filter.SetBoundaryEdges(property.get());
        return true;
    }
    BoolProperty property{"BoundaryEdges", "BoundaryEdges",
                          R"(If the value of this property is set to 1, boundary
edges will be extracted. Boundary edges are defined as lines cells or
edges that are used by only one polygon.)"_help,
                          true};
};

struct Wrapper1 {
    bool set(vtkFeatureEdges& filter) {
        filter.SetFeatureEdges(property.get());
        return true;
    }
    BoolProperty property{"FeatureEdges", "FeatureEdges",
                          R"(If the value of this property is set to 1, feature edges
will be extracted. Feature edges are defined as edges that are used by
two polygons whose dihedral angle is greater than the feature angle.
(See the FeatureAngle property.) Toggle whether to extract feature
edges.)"_help,
                          true};
};

struct Wrapper2 {
    bool set(vtkFeatureEdges& filter) {
        filter.SetNonManifoldEdges(property.get());
        return true;
    }
    BoolProperty property{"NonManifoldEdges", "Non-Manifold Edges",
                          R"(If the value of this property is set to 1, non-manifold
edges will be extracted. Non-manifold edges are defined as edges that
are use by three or more polygons.)"_help,
                          true};
};

struct Wrapper3 {
    bool set(vtkFeatureEdges& filter) {
        filter.SetManifoldEdges(property.get());
        return true;
    }
    BoolProperty property{"ManifoldEdges", "ManifoldEdges",
                          R"(If the value of this property is set to 1, manifold
edges will be extracted. Manifold edges are defined as edges that are
used by exactly two polygons.)"_help,
                          false};
};

struct Wrapper4 {
    bool set(vtkFeatureEdges& filter) {
        filter.SetColoring(property.get());
        return true;
    }
    BoolProperty property{"Coloring", "Coloring",
                          R"(If the value of this property is set to 1, then the
extracted edges are assigned a scalar value based on the type of the
edge.)"_help,
                          false};
};

struct Wrapper5 {
    bool set(vtkFeatureEdges& filter) {
        filter.SetFeatureAngle(property.get());
        return true;
    }
    DoubleProperty property{"FeatureAngle",
                            "FeatureAngle",
                            R"(The value of this property is used to define a feature
edge. If the surface normal between two adjacent triangles is at least
as large as this Feature Angle, a feature edge exists. (See the
FeatureEdges property.))"_help,
                            30.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{180.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkFeatureEdges> {
    static constexpr std::string_view uri = "vtk.vtkFeatureEdges";
    static constexpr std::string_view className = "vtkFeatureEdges";
    static constexpr std::string_view identifier = "FeatureEdges";
    static constexpr std::string_view displayName = "Feature Edges";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkPolyData", -1, R"(This property specifies the input to the Feature Edges
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;
    static constexpr std::string_view doc =
        R"(The Feature Edges filter extracts various subsets of edges
from the input data set. This filter operates on polygonal
data and produces polygonal output.)";
};

void registervtkFeatureEdges(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkFeatureEdges>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
