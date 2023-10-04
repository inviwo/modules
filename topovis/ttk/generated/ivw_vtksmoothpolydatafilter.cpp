#include "ivw_vtksmoothpolydatafilter.h"

#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/ttk/processors/ttkgenericprocessor.h>
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
#include <vtkSmoothPolyDataFilter.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkSmoothPolyDataFilter& filter) {
        filter.SetNumberOfIterations(property.get());
        return true;
    }
    IntProperty property{"NumberOfIterations",
                         "Number of Iterations",
                         R"(This property sets the maximum number of smoothing
iterations to perform. More iterations produce better
smoothing.)"_help,
                         20,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkSmoothPolyDataFilter& filter) {
        filter.SetConvergence(property.get());
        return true;
    }
    DoubleProperty property{"Convergence",
                            "Convergence",
                            R"(The value of this property limits the maximum motion of
any point. It is expressed as a fraction of the length of the diagonal
of the bounding box of the input dataset. If the maximum point motion
during a smoothing iteration is less than the value of this property,
the smoothing operation terminates.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkSmoothPolyDataFilter> {
    static constexpr std::string_view className = "vtkSmoothPolyDataFilter";
    static constexpr std::string_view identifier = "SmoothPolyDataFilter";
    static constexpr std::string_view displayName = "Smooth";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkPolyData", -1, R"(This property specifies the input to the Smooth
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;
    static constexpr std::string_view doc =
        R"(The Smooth filter operates on a polygonal data set by
iteratively adjusting the position of the points using
Laplacian smoothing. (Because this filter only adjusts
point positions, the output data set is also polygonal.)
This results in better-shaped cells and more evenly
distributed points. The Convergence slider limits the
maximum motion of any point. It is expressed as a fraction
of the length of the diagonal of the bounding box of the
data set. If the maximum point motion during a smoothing
iteration is less than the Convergence value, the
smoothing operation terminates.)";
};

void registervtkSmoothPolyDataFilter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkSmoothPolyDataFilter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
