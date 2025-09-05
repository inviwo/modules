#include "ivw_vtkdelaunay2d.h"
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
#include <vtkDelaunay2D.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkDelaunay2D& filter) {
        filter.SetProjectionPlaneMode(property.get());
        return true;
    }
    OptionPropertyInt property{
        "ProjectionPlaneMode",
        "ProjectionPlaneMode",
        R"(This property determines type of projection plane to use
in performing the triangulation.)"_help,
        {{"XY Plane", "XY Plane", 0}, {"Best-Fitting Plane", "Best-Fitting Plane", 2}},
        0};
};

struct Wrapper1 {
    bool set(vtkDelaunay2D& filter) {
        filter.SetAlpha(property.get());
        return true;
    }
    DoubleProperty property{"Alpha",
                            "Alpha",
                            R"(The value of this property controls the output of this
filter. For a non-zero alpha value, only edges or triangles contained
within a sphere centered at mesh vertices will be output. Otherwise,
only triangles will be output.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkDelaunay2D& filter) {
        filter.SetTolerance(property.get());
        return true;
    }
    DoubleProperty property{"Tolerance",
                            "Tolerance",
                            R"(This property specifies a tolerance to control
discarding of closely spaced points. This tolerance is specified as a
fraction of the diagonal length of the bounding box of the
points.)"_help,
                            1e-05,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkDelaunay2D& filter) {
        filter.SetOffset(property.get());
        return true;
    }
    DoubleProperty property{"Offset",
                            "Offset",
                            R"(This property is a multiplier to control the size of the
initial, bounding Delaunay triangulation.)"_help,
                            1.0,
                            std::pair{0.75, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(vtkDelaunay2D& filter) {
        filter.SetBoundingTriangulation(property.get());
        return true;
    }
    BoolProperty property{"BoundingTriangulation", "BoundingTriangulation",
                          R"(If this property is set to 1, bounding triangulation
points (and associated triangles) are included in the output. These are
introduced as an initial triangulation to begin the triangulation
process. This feature is nice for debugging output.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkDelaunay2D> {
    static constexpr std::string_view uri = "vtk.vtkDelaunay2D";
    static constexpr std::string_view className = "vtkDelaunay2D";
    static constexpr std::string_view identifier = "Delaunay2D";
    static constexpr std::string_view displayName = "Delaunay 2D";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkPointSet",
                  .numComp = -1,
                  .doc = R"(This property specifies the input dataset to the
Delaunay 2D filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;

    static constexpr std::string_view doc =
        R"ivw(Delaunay2D is a filter that constructs a 2D Delaunay
triangulation from a list of input points. These points
may be represented by any dataset of type vtkPointSet and
subclasses. The output of the filter is a polygonal
dataset containing a triangle mesh. The 2D Delaunay
triangulation is defined as the triangulation that
satisfies the Delaunay criterion for n-dimensional
simplexes (in this case n=2 and the simplexes are
triangles). This criterion states that a circumsphere of
each simplex in a triangulation contains only the n+1
defining points of the simplex. In two dimensions, this
translates into an optimal triangulation. That is, the
maximum interior angle of any triangle is less than or
equal to that of any possible triangulation. Delaunay
triangulations are used to build topological structures
from unorganized (or unstructured) points. The input to
this filter is a list of points specified in 3D, even
though the triangulation is 2D. Thus the triangulation is
constructed in the x-y plane, and the z coordinate is
ignored (although carried through to the output). You can
use the option ProjectionPlaneMode in order to compute the
best-fitting plane to the set of points, project the
points and that plane and then perform the triangulation
using their projected positions and then use it as the
plane in which the triangulation is performed. The
Delaunay triangulation can be numerically sensitive in
some cases. To prevent problems, try to avoid injecting
points that will result in triangles with bad aspect
ratios (1000:1 or greater). In practice this means
inserting points that are "widely dispersed", and enables
smooth transition of triangle sizes throughout the mesh.
(You may even want to add extra points to create a better
point distribution.) If numerical problems are present,
you will see a warning message to this effect at the end
of the triangulation process. Warning: Points arranged on
a regular lattice (termed degenerate cases) can be
triangulated in more than one way (at least according to
the Delaunay criterion). The choice of triangulation (as
implemented by this algorithm) depends on the order of the
input points. The first three points will form a triangle;
other degenerate points will not break this triangle.
Points that are coincident (or nearly so) may be discarded
by the algorithm. This is because the Delaunay
triangulation requires unique input points. The output of
the Delaunay triangulation is supposedly a convex hull. In
certain cases this implementation may not generate the
convex hull.)ivw";
};

void registervtkDelaunay2D(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkDelaunay2D>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
