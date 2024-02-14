#include "ivw_vtkppolydatanormals.h"
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
#include <vtkPPolyDataNormals.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkPPolyDataNormals& filter) {
        filter.SetFeatureAngle(property.get());
        return true;
    }
    DoubleProperty property{"FeatureAngle",
                            "FeatureAngle",
                            R"(The value of this property defines a feature edge. If
the surface normal between two adjacent triangles is at least as large
as this Feature Angle, a feature edge exists. If Splitting is on,
points are duplicated along these feature edges. (See the Splitting
property.))"_help,
                            30.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{180.0, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkPPolyDataNormals& filter) {
        filter.SetSplitting(property.get());
        return true;
    }
    BoolProperty property{"Splitting", "Splitting",
                          R"(This property controls the splitting of sharp edges. If
sharp edges are split (property value = 1), then points are duplicated
along these edges, and separate normals are computed for both sets of
points to give crisp (rendered) surface definition.)"_help,
                          true};
};

struct Wrapper2 {
    bool set(vtkPPolyDataNormals& filter) {
        filter.SetConsistency(property.get());
        return true;
    }
    BoolProperty property{"Consistency", "Consistency",
                          R"(The value of this property controls whether consistent
polygon ordering is enforced. Generally the normals for a data set
should either all point inward or all point outward. If the value of
this property is 1, then this filter will reorder the points of cells
that whose normal vectors are oriented the opposite direction from the
rest of those in the data set.)"_help,
                          true};
};

struct Wrapper3 {
    bool set(vtkPPolyDataNormals& filter) {
        filter.SetFlipNormals(property.get());
        return true;
    }
    BoolProperty property{"FlipNormals", "FlipNormals",
                          R"(If the value of this property is 1, this filter will
reverse the normal direction (and reorder the points accordingly) for
all polygons in the data set; this changes front-facing polygons to
back-facing ones, and vice versa. You might want to do this if your
viewing position will be inside the data set instead of outside of
it.)"_help,
                          false};
};

struct Wrapper4 {
    bool set(vtkPPolyDataNormals& filter) {
        filter.SetNonManifoldTraversal(property.get());
        return true;
    }
    BoolProperty property{"NonManifoldTraversal", "Non-Manifold Traversal",
                          R"(Turn on/off traversal across non-manifold edges. Not
traversing non-manifold edges will prevent problems where the
consistency of polygonal ordering is corrupted due to topological
loops.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(vtkPPolyDataNormals& filter) {
        filter.SetComputeCellNormals(property.get());
        return true;
    }
    BoolProperty property{"ComputeCellNormals", "ComputeCellNormals",
                          R"(This filter computes the normals at the points in the
data set. In the process of doing this it computes polygon normals too.
If you want these normals to be passed to the output of this filter,
set the value of this property to 1.)"_help,
                          false};
};

struct Wrapper6 {
    bool set(vtkPPolyDataNormals& filter) {
        filter.SetPieceInvariant(property.get());
        return true;
    }
    BoolProperty property{"PieceInvariant", "PieceInvariant",
                          R"(Turn this option to to produce the same results
regardless of the number of processors used (i.e., avoid seams along
processor boundaries). Turn this off if you do want to process ghost
levels and do not mind seams.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkPPolyDataNormals> {
    static constexpr std::string_view uri = "vtk.vtkPPolyDataNormals";
    static constexpr std::string_view className = "vtkPPolyDataNormals";
    static constexpr std::string_view identifier = "PolyDataNormals";
    static constexpr std::string_view displayName = "Generate Surface Normals";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkPolyData", -1, R"(This property specifies the input to the Normals
Generation filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;
    static constexpr std::string_view doc = R"(This filter
generates surface normals at the points of the input
polygonal dataset to provide smooth shading of the
dataset. The resulting dataset is also polygonal. The
filter works by calculating a normal vector for each
polygon in the dataset and then averaging the normals at
the shared points.)";
};

void registervtkPPolyDataNormals(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkPPolyDataNormals>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
