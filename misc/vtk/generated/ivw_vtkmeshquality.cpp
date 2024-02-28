#include "ivw_vtkmeshquality.h"
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
#include <vtkMeshQuality.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkMeshQuality& filter) {
        filter.SetTriangleQualityMeasure(property.get());
        return true;
    }
    OptionPropertyInt property{"TriangleQualityMeasure",
                               "TriangleQualityMeasure",
                               R"(This property indicates which quality measure will be
used to evaluate triangle quality. The radius ratio is the size of a
circle circumscribed by a triangle's 3 vertices divided by the size of
a circle tangent to a triangle's 3 edges. The edge ratio is the ratio
of the longest edge length to the shortest edge length.)"_help,
                               {{"Area", "Area", 28},
                                {"Aspect Ratio", "Aspect Ratio", 1},
                                {"Aspect Frobenius", "Aspect Frobenius", 3},
                                {"Condition", "Condition", 9},
                                {"Distortion", "Distortion", 15},
                                {"Edge Ratio", "Edge Ratio", 0},
                                {"Maximum Angle", "Maximum Angle", 8},
                                {"Minimum Angle", "Minimum Angle", 6},
                                {"Scaled Jacobian", "Scaled Jacobian", 10},
                                {"Radius Ratio", "Radius Ratio", 2},
                                {"Relative Size Squared", "Relative Size Squared", 12},
                                {"Shape", "Shape", 13},
                                {"Shape and Size", "Shape and Size", 14}},
                               9};
};

struct Wrapper1 {
    bool set(vtkMeshQuality& filter) {
        filter.SetQuadQualityMeasure(property.get());
        return true;
    }
    OptionPropertyInt property{"QuadQualityMeasure",
                               "QuadQualityMeasure",
                               R"(This property indicates which quality measure will be
used to evaluate quadrilateral quality.)"_help,
                               {{"Area", "Area", 28},
                                {"Aspect Ratio", "Aspect Ratio", 1},
                                {"Condition", "Condition", 9},
                                {"Distortion", "Distortion", 15},
                                {"Edge Ratio", "Edge Ratio", 0},
                                {"Jacobian", "Jacobian", 25},
                                {"Maximum Aspect Frobenius", "Maximum Aspect Frobenius", 5},
                                {"Maximum Aspect Frobenius", "Maximum Aspect Frobenius", 5},
                                {"Maximum Edge Ratio", "Maximum Edge Ratio", 16},
                                {"Mean Aspect Frobenius", "Mean Aspect Frobenius", 4},
                                {"Minimum Angle", "Minimum Angle", 6},
                                {"Oddy", "Oddy", 23},
                                {"Radius Ratio", "Radius Ratio", 2},
                                {"Relative Size Squared", "Relative Size Squared", 12},
                                {"Scaled Jacobian", "Scaled Jacobian", 10},
                                {"Shape", "Shape", 13},
                                {"Shape and Size", "Shape and Size", 14},
                                {"Shear", "Shear", 11},
                                {"Shear and Size", "Shear and Size", 24},
                                {"Skew", "Skew", 17},
                                {"Stretch", "Stretch", 20},
                                {"Taper", "Taper", 18},
                                {"Warpage", "Warpage", 26}},
                               4};
};

struct Wrapper2 {
    bool set(vtkMeshQuality& filter) {
        filter.SetTetQualityMeasure(property.get());
        return true;
    }
    OptionPropertyInt property{"TetQualityMeasure",
                               "TetQualityMeasure",
                               R"(This property indicates which quality measure will be
used to evaluate tetrahedral quality. The radius ratio is the size of a
sphere circumscribed by a tetrahedron's 4 vertices divided by the size
of a circle tangent to a tetrahedron's 4 faces. The edge ratio is the
ratio of the longest edge length to the shortest edge length. The
collapse ratio is the minimum ratio of height of a vertex above the
triangle opposite it divided by the longest edge of the opposing
triangle across all vertex/triangle pairs.)"_help,
                               {{"Edge Ratio", "Edge Ratio", 0},
                                {"Aspect Beta", "Aspect Beta", 29},
                                {"Aspect Gamma", "Aspect Gamma", 27},
                                {"Aspect Frobenius", "Aspect Frobenius", 3},
                                {"Aspect Ratio", "Aspect Ratio", 1},
                                {"Collapse Ratio", "Collapse Ratio", 7},
                                {"Condition", "Condition", 9},
                                {"Distortion", "Distortion", 15},
                                {"Jacobian", "Jacobian", 25},
                                {"Minimum Dihedral Angle", "Minimum Dihedral Angle", 6},
                                {"Radius Ratio", "Radius Ratio", 2},
                                {"Relative Size Squared", "Relative Size Squared", 12},
                                {"Scaled Jacobian", "Scaled Jacobian", 10},
                                {"Shape", "Shape", 13},
                                {"Shape and Size", "Shape and Size", 14},
                                {"Volume", "Volume", 19}},
                               10};
};

struct Wrapper3 {
    bool set(vtkMeshQuality& filter) {
        filter.SetHexQualityMeasure(property.get());
        return true;
    }
    OptionPropertyInt property{"HexQualityMeasure",
                               "HexQualityMeasure",
                               R"(This property indicates which quality measure will be
used to evaluate hexahedral quality.)"_help,
                               {{"Diagonal", "Diagonal", 21},
                                {"Dimension", "Dimension", 22},
                                {"Distortion", "Distortion", 15},
                                {"Edge Ratio", "Edge Ratio", 0},
                                {"Jacobian", "Jacobian", 25},
                                {"Maximum Edge Ratio", "Maximum Edge Ratio", 16},
                                {"Maximum Aspect Frobenius", "Maximum Aspect Frobenius", 5},
                                {"Mean Aspect Frobenius", "Mean Aspect Frobenius", 4},
                                {"Oddy", "Oddy", 23},
                                {"Relative Size Squared", "Relative Size Squared", 12},
                                {"Scaled Jacobian", "Scaled Jacobian", 10},
                                {"Shape", "Shape", 13},
                                {"Shape and Size", "Shape and Size", 14},
                                {"Shear", "Shear", 11},
                                {"Shear and Size", "Shear and Size", 24},
                                {"Skew", "Skew", 17},
                                {"Stretch", "Stretch", 20},
                                {"Taper", "Taper", 18},
                                {"Volume", "Volume", 19}},
                               6};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkMeshQuality> {
    static constexpr std::string_view uri = "vtk.vtkMeshQuality";
    static constexpr std::string_view className = "vtkMeshQuality";
    static constexpr std::string_view identifier = "MeshQuality";
    static constexpr std::string_view displayName = "Mesh Quality";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataSet", -1, R"(This property specifies the input to the Mesh Quality
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;
    static constexpr std::string_view doc = R"(This filter
creates a new cell array containing a geometric measure of
each cell's fitness. Different quality measures can be
chosen for different cell shapes. Supported shapes include linear
triangles, quadrilaterals, tetrahedra, and hexahedra. For
other shapes, a value of 0 is assigned.)";
};

void registervtkMeshQuality(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkMeshQuality>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
