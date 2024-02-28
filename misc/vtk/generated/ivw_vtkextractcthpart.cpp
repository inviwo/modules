#include "ivw_vtkextractcthpart.h"
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
#include <vtkExtractCTHPart.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkExtractCTHPart& filter) {
        filter.SetVolumeFractionSurfaceValue(property.get());
        return true;
    }
    DoubleProperty property{"VolumeFractionSurfaceValue",
                            "Volume Fraction Value",
                            R"(The value of this property is the volume fraction value
for the surface.)"_help,
                            0.1,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkExtractCTHPart& filter) {
        filter.SetCapping(property.get());
        return true;
    }
    BoolProperty property{"CapSurfaces", "CapSurfaces",
                          R"(When enabled, volume surfaces are capped to produce visually closed
surface.)"_help,
                          true};
};

struct Wrapper2 {
    bool set(vtkExtractCTHPart& filter) {
        filter.SetRemoveGhostCells(property.get());
        return true;
    }
    BoolProperty property{"RemoveGhostCells", "RemoveGhostCells",
                          R"(When set to false, the output surfaces will not hide contours
extracted from ghost cells. This results in overlapping contours but
overcomes holes.  Default is set to true.)"_help,
                          true};
};

struct Wrapper3 {
    bool set(vtkExtractCTHPart& filter) {
        filter.SetGenerateTriangles(property.get());
        return true;
    }
    BoolProperty property{
        "GenerateTriangles", "GenerateTriangles",
        R"(Triangulate results. When set to false, the internal cut and contour filters
are told not to triangulate results if possible.)"_help,
        false};
};

struct Wrapper4 {
    bool set(vtkExtractCTHPart& filter) {
        filter.SetGenerateSolidGeometry(property.get());
        return true;
    }
    BoolProperty property{
        "GenerateSolidGeometry", "GenerateSolidGeometry",
        R"(Generate solid output with 3D cells. When set to false, 2D contours are generated.)"_help,
        false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkExtractCTHPart> {
    static constexpr std::string_view uri = "vtk.vtkExtractCTHPart";
    static constexpr std::string_view className = "vtkExtractCTHPart";
    static constexpr std::string_view identifier = "CTHPart";
    static constexpr std::string_view displayName = "Extract CTH Parts";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", 1, R"(This property specifies the input to the Extract CTH
Parts filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;
    static constexpr std::string_view doc = R"(Extract
CTH Parts is a specialized filter for visualizing the data
from a CTH simulation. It first converts the selected
cell-centered arrays to point-centered ones. It then
contours each array at a value of 0.5. The user has the
option of clipping the resulting surface(s) with a plane.
This filter only operates on unstructured data. It
produces polygonal output.)";
};

void registervtkExtractCTHPart(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkExtractCTHPart>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
