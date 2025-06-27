#include "ivw_vtkextractenclosedpoints.h"
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
#include <vtkExtractEnclosedPoints.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkExtractEnclosedPoints& filter) {
        filter.SetTolerance(property.get());
        return true;
    }
    DoubleProperty property{
        "Tolerance",
        "Tolerance",
        R"(Specify the tolerance on the intersection. The tolerance is expressed as
a fraction of the diagonal of the bounding box of the enclosing surface.)"_help,
        0.0001,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkExtractEnclosedPoints& filter) {
        filter.SetCheckSurface(property.get());
        return true;
    }
    BoolProperty property{"CheckSurface", "CheckSurface",
                          R"(Specify whether to check the surface for closure. If on, then the
algorithm first checks to see if the surface is closed and manifold.)"_help,
                          false};
};

struct Wrapper2 {
    bool set(vtkExtractEnclosedPoints& filter) {
        filter.SetGenerateOutliers(property.get());
        return true;
    }
    BoolProperty property{"GenerateOutliers", "GenerateOutliers",
                          R"(If checked, then a second output will be created
that contains the outlier points.)"_help,
                          false};
};

struct Wrapper3 {
    bool set(vtkExtractEnclosedPoints& filter) {
        filter.SetGenerateVertices(property.get());
        return true;
    }
    BoolProperty property{
        "GenerateVertices", "GenerateVertices",
        R"(If checked, then the outputs will contain a poly-vertex cell. This takes a
lot more memory, however certain filters many requires cells to be present
to function properly.)"_help,
        true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkExtractEnclosedPoints> {
    static constexpr std::string_view uri = "vtk.vtkExtractEnclosedPoints";
    static constexpr std::string_view className = "vtkExtractEnclosedPoints";
    static constexpr std::string_view identifier = "ExtractEnclosedPoints";
    static constexpr std::string_view displayName = "Extract Enclosed Points";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Input", "vtkPointSet", -1,
                  R"(This property specifies the input to extract points from.)"},
        InputData{"Surface", "vtkPolyData", -1,
                  R"(This property specifies the closed and manifold surface.)"}};
    inline static std::array<OutputData, 2> outports = {OutputData{"outport0", "EnclosedPoints", 0},
                                                        OutputData{"outport1", "Outliers", 1}};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;

    static constexpr std::string_view doc =
        R"ivw(Evaluates all points in the input dataset to determine whether they are contained within
an enclosing surface, defined by the **Surface**. The filter assumes that the surface
is closed and manifold. To force a check to ensure these preconditions are met
set the **CheckSurface** property.)ivw";
};

void registervtkExtractEnclosedPoints(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkExtractEnclosedPoints>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
