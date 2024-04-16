#include "ivw_vtkpmaskpoints.h"
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
#include <vtkPMaskPoints.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkPMaskPoints& filter) {
        filter.SetOnRatio(property.get());
        return true;
    }
    IntProperty property{"OnRatio",
                         "OnRatio",
                         R"(The value of this property specifies that every
OnStride-th points will be retained in the output when not using Random
(the skip or stride size for point ids). (For example, if the on ratio
is 3, then the output will contain every 3rd point, up to the the
maximum number of points.))"_help,
                         2,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkPMaskPoints& filter) {
        filter.SetMaximumNumberOfPoints(property.get());
        return true;
    }
    IntProperty property{"MaximumNumberOfPoints",
                         "Maximum Number of Points",
                         R"(The value of this property indicates the maximum number
of points in the output dataset.)"_help,
                         5000,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkPMaskPoints& filter) {
        filter.SetProportionalMaximumNumberOfPoints(property.get());
        return true;
    }
    BoolProperty property{"ProportionalMaximumNumberOfPoints",
                          "Proportionally Distribute Maximum Number Of Points",
                          R"(When this is off, the maximum number of points is taken
per processor when running in parallel (total number of points = number
of processors * maximum number of points). When this is on, the maximum
number of points is proportionally distributed across processors
depending on the number of points per processor
("total number of points" is the same as "maximum number of points"
maximum number of points per processor = number of points on a processor
* maximum number of points / total number of points across all processors
).)"_help,
                          false};
};

struct Wrapper3 {
    bool set(vtkPMaskPoints& filter) {
        filter.SetOffset(property.get());
        return true;
    }
    IntProperty property{"Offset",
                         "Offset",
                         R"(The value of this property indicates the starting point
id in the ordered list of input points from which to start
masking.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(vtkPMaskPoints& filter) {
        filter.SetRandomMode(property.get());
        return true;
    }
    BoolProperty property{"RandomMode", "Random Sampling",
                          R"(If the value of this property is set to true, then the
points in the output will be randomly selected from the input in
various ways set by Random Mode; otherwise this filter will subsample
point ids regularly.)"_help,
                          false};
};

struct Wrapper5 {
    bool set(vtkPMaskPoints& filter) {
        filter.SetRandomModeType(property.get());
        return true;
    }
    OptionPropertyInt property{
        "RandomModeType",
        "Random Sampling Mode",
        R"(Randomized Id Strides picks points with random id
increments starting at Offset (the output probably isn't a
statistically random sample). Random Sampling generates a statistically
random sample of the input, ignoring Offset (fast - O(sample size)).
Spatially Stratified Random Sampling is a variant of random sampling
that splits the points into equal sized spatial strata before randomly
sampling (slow - O(N log N)).)"_help,
        {{"Randomized Id Strides", "Randomized Id Strides", 0},
         {"Random Sampling", "Random Sampling", 1},
         {"Spatially Stratified Random Sampling", "Spatially Stratified Random Sampling", 2}},
        0};
};

struct Wrapper6 {
    bool set(vtkPMaskPoints& filter) {
        filter.SetGenerateVertices(property.get());
        return true;
    }
    BoolProperty property{"GenerateVertices", "GenerateVertices",
                          R"(This property specifies whether to generate vertex cells
as the topography of the output. If set to 1, the geometry (vertices)
will be displayed in the rendering window; otherwise no geometry will
be displayed.)"_help,
                          false};
};

struct Wrapper7 {
    bool set(vtkPMaskPoints& filter) {
        filter.SetSingleVertexPerCell(property.get());
        return true;
    }
    BoolProperty property{"SingleVertexPerCell", "SingleVertexPerCell",
                          R"(Tell filter to only generate one vertex per cell instead
of multiple vertices in one cell.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkPMaskPoints> {
    static constexpr std::string_view uri = "vtk.vtkPMaskPoints";
    static constexpr std::string_view className = "vtkPMaskPoints";
    static constexpr std::string_view identifier = "MaskPoints";
    static constexpr std::string_view displayName = "Mask Points";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(This property specifies the input to the Mask Points
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;

    static constexpr std::string_view doc = R"(The Mask Points
filter reduces the number of points in the dataset. It
operates on any type of dataset, but produces only points
/ vertices as output.)";
};

void registervtkPMaskPoints(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkPMaskPoints>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
