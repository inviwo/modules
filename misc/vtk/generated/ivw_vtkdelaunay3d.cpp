#include "ivw_vtkdelaunay3d.h"
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
#include <vtkDelaunay3D.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkDelaunay3D& filter) {
        filter.SetAlpha(property.get());
        return true;
    }
    DoubleProperty property{"Alpha",
                            "Alpha",
                            R"(This property specifies the alpha (or distance) value to
control the output of this filter. For a non-zero alpha value, only
edges, faces, or tetra contained within the circumsphere (of radius
alpha) will be output. Otherwise, only tetrahedra will be
output.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkDelaunay3D& filter) {
        filter.SetTolerance(property.get());
        return true;
    }
    DoubleProperty property{"Tolerance",
                            "Tolerance",
                            R"(This property specifies a tolerance to control
discarding of closely spaced points. This tolerance is specified as a
fraction of the diagonal length of the bounding box of the
points.)"_help,
                            0.001,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkDelaunay3D& filter) {
        filter.SetOffset(property.get());
        return true;
    }
    DoubleProperty property{"Offset",
                            "Offset",
                            R"(This property specifies a multiplier to control the size
of the initial, bounding Delaunay triangulation.)"_help,
                            2.5,
                            std::pair{2.5, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkDelaunay3D& filter) {
        filter.SetBoundingTriangulation(property.get());
        return true;
    }
    BoolProperty property{"BoundingTriangulation", "BoundingTriangulation",
                          R"(This boolean controls whether bounding triangulation
points (and associated triangles) are included in the output. (These
are introduced as an initial triangulation to begin the triangulation
process. This feature is nice for debugging output.))"_help,
                          false};
};

struct Wrapper4 {
    bool set(vtkDelaunay3D& filter) {
        filter.SetAlphaTets(property.get());
        return true;
    }
    BoolProperty property{"AlphaTets", "AlphaTets",
                          R"(This boolean controls whether tetrahedrons which satisfy
the alpha criterion output when alpha is non-zero.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(vtkDelaunay3D& filter) {
        filter.SetAlphaTris(property.get());
        return true;
    }
    BoolProperty property{"AlphaTris", "AlphaTris",
                          R"(This boolean controls whether triangles which satisfy
the alpha criterion output when alpha is non-zero.)"_help,
                          true};
};

struct Wrapper6 {
    bool set(vtkDelaunay3D& filter) {
        filter.SetAlphaLines(property.get());
        return true;
    }
    BoolProperty property{"AlphaLines", "AlphaLines",
                          R"(This boolean controls whether lines which satisfy the
alpha criterion output when alpha is non-zero.)"_help,
                          false};
};

struct Wrapper7 {
    bool set(vtkDelaunay3D& filter) {
        filter.SetAlphaVerts(property.get());
        return true;
    }
    BoolProperty property{"AlphaVerts", "AlphaVerts",
                          R"(This boolean controls whether vertices which satisfy the
alpha criterion are output when alpha is non-zero.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkDelaunay3D> {
    static constexpr std::string_view uri = "vtk.vtkDelaunay3D";
    static constexpr std::string_view className = "vtkDelaunay3D";
    static constexpr std::string_view identifier = "Delaunay3D";
    static constexpr std::string_view displayName = "Delaunay 3D";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkPointSet",
                  .numComp = -1,
                  .doc = R"(This property specifies the input dataset to the
Delaunay 3D filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;

    static constexpr std::string_view doc = R"ivw(Delaunay3D is a filter that constructs
a 3D Delaunay triangulation from a list of input points. These points may be
represented by any dataset of type vtkPointSet and subclasses. The output of
the filter is an unstructured grid dataset. Usually the output is a tetrahedral
mesh, but if a non-zero alpha distance value is specified (called the "alpha"
value), then only tetrahedra, triangles, edges, and vertices lying within the
alpha radius are output. In other words, non-zero alpha values may result in
arbitrary combinations of tetrahedra, triangles, lines, and vertices. (The
notion of alpha value is derived from Edelsbrunner's work on "alpha shapes".)
The 3D Delaunay triangulation is defined as the triangulation that satisfies
the Delaunay criterion for n-dimensional simplexes (in this case n=3 and the
simplexes are tetrahedra). This criterion states that a circumsphere of each
simplex in a triangulation contains only the n+1 defining points of the
simplex. (See text for more information.) While in two dimensions this
translates into an "optimal" triangulation, this is not true in 3D, since a
measurement for optimality in 3D is not agreed on. Delaunay triangulations are
used to build topological structures from unorganized (or unstructured) points.
The input to this filter is a list of points specified in 3D. (If you wish to
create 2D triangulations see Delaunay2D.) The output is an unstructured grid.
The Delaunay triangulation can be numerically sensitive. To prevent problems,
try to avoid injecting points that will result in triangles with bad aspect
ratios (1000:1 or greater). In practice this means inserting points that are
"widely dispersed", and enables smooth transition of triangle sizes throughout
the mesh. (You may even want to add extra points to create a better point
distribution.) If numerical problems are present, you will see a warning
message to this effect at the end of the triangulation process. Warning: Points
arranged on a regular lattice (termed degenerate cases) can be triangulated in
more than one way (at least according to the Delaunay criterion). The choice of
triangulation (as implemented by this algorithm) depends on the order of the
input points. The first four points will form a tetrahedron; other degenerate
points (relative to this initial tetrahedron) will not break it. Points that
are coincident (or nearly so) may be discarded by the algorithm. This is
because the Delaunay triangulation requires unique input points. You can
control the definition of coincidence with the "Tolerance" instance variable.
The output of the Delaunay triangulation is supposedly a convex hull. In
certain cases this implementation may not generate the convex hull. This
behavior can be controlled by the Offset instance variable. Offset is a
multiplier used to control the size of the initial triangulation. The larger
the offset value, the more likely you will generate a convex hull; and the more
likely you are to see numerical problems. The implementation of this algorithm
varies from the 2D Delaunay algorithm (i.e., Delaunay2D) in an important way.
When points are injected into the triangulation, the search for the enclosing
tetrahedron is quite different. In the 3D case, the closest previously inserted
point point is found, and then the connected tetrahedra are searched to find
the containing one. (In 2D, a "walk" towards the enclosing triangle is
performed.) If the triangulation is Delaunay, then an enclosing tetrahedron
will be found. However, in degenerate cases an enclosing tetrahedron may not be
found and the point will be rejected.)ivw";
};

void registervtkDelaunay3D(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkDelaunay3D>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
