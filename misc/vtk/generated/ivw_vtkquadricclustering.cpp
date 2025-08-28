#include "ivw_vtkquadricclustering.h"
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
#include <vtkQuadricClustering.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkQuadricClustering& filter) {
        filter.SetNumberOfDivisions(property.get(0), property.get(1), property.get(2));
        return true;
    }
    IntVec3Property property{"NumberOfDivisions",
                             "Number of Dimensions",
                             R"(This property specifies the number of bins along the X,
Y, and Z axes of the data set.)"_help,
                             ivec3{50, 50, 50},
                             std::pair{ivec3{0, 0, 0}, ConstraintBehavior::Ignore},
                             std::pair{ivec3{100, 100, 100}, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkQuadricClustering& filter) {
        filter.SetUseInputPoints(property.get());
        return true;
    }
    BoolProperty property{"UseInputPoints", "UseInputPoints",
                          R"(If the value of this property is set to 1, the
representative point for each bin is selected from one of the input
points that lies in that bin; the input point that produces the least
error is chosen. If the value of this property is 0, the location of
the representative point is calculated to produce the least error
possible for that bin, but the point will most likely not be one of the
input points.)"_help,
                          true};
};

struct Wrapper2 {
    bool set(vtkQuadricClustering& filter) {
        filter.SetUseFeatureEdges(property.get());
        return true;
    }
    BoolProperty property{"UseFeatureEdges", "UseFeatureEdges",
                          R"(If this property is set to 1, feature edge quadrics will
be used to maintain the boundary edges along processor
divisions.)"_help,
                          false};
};

struct Wrapper3 {
    bool set(vtkQuadricClustering& filter) {
        filter.SetUseFeaturePoints(property.get());
        return true;
    }
    BoolProperty property{"UseFeaturePoints", "UseFeaturePoints",
                          R"(If this property is set to 1, feature point quadrics
will be used to maintain the boundary points along processor
divisions.)"_help,
                          false};
};

struct Wrapper4 {
    bool set(vtkQuadricClustering& filter) {
        filter.SetCopyCellData(property.get());
        return true;
    }
    BoolProperty property{"CopyCellData", "CopyCellData",
                          R"(If this property is set to 1, the cell data from the
input will be copied to the output.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(vtkQuadricClustering& filter) {
        filter.SetUseInternalTriangles(property.get());
        return true;
    }
    BoolProperty property{"UseInternalTriangles", "UseInternalTriangles",
                          R"(If this property is set to 1, triangles completely
contained in a spatial bin will be included in the computation of the
bin's quadrics. When this property is set to 0, the filters operates
faster, but the resulting surface may not be as
well-behaved.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkQuadricClustering> {
    static constexpr std::string_view uri = "vtk.vtkQuadricClustering";
    static constexpr std::string_view className = "vtkQuadricClustering";
    static constexpr std::string_view identifier = "QuadricClustering";
    static constexpr std::string_view displayName = "Quadric Clustering";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkPolyData",
                  .numComp = -1,
                  .doc = R"(This property specifies the input to the Quadric
Clustering filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;

    static constexpr std::string_view doc = R"ivw(The Quadric
Clustering filter produces a reduced-resolution polygonal
approximation of the input polygonal dataset. This filter
is the one used by ParaView for computing LODs. It uses
spatial binning to reduce the number of points in the data
set; points that lie within the same spatial bin are
collapsed into one representative point.)ivw";
};

void registervtkQuadricClustering(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkQuadricClustering>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
