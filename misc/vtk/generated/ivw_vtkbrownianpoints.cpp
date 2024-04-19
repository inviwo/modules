#include "ivw_vtkbrownianpoints.h"
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
#include <vtkBrownianPoints.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkBrownianPoints& filter) {
        filter.SetMinimumSpeed(property.get());
        return true;
    }
    DoubleProperty property{"MinimumSpeed",
                            "MinimumSpeed",
                            R"(This property specifies the minimum length of the random
point vectors generated.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkBrownianPoints& filter) {
        filter.SetMaximumSpeed(property.get());
        return true;
    }
    DoubleProperty property{"MaximumSpeed",
                            "MaximumSpeed",
                            R"(This property specifies the maximum length of the random
point vectors generated.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkBrownianPoints> {
    static constexpr std::string_view uri = "vtk.vtkBrownianPoints";
    static constexpr std::string_view className = "vtkBrownianPoints";
    static constexpr std::string_view identifier = "BrownianPoints";
    static constexpr std::string_view displayName = "Random Vectors";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataSet", -1, R"(This property specifies the input to the Random Vectors
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;

    static constexpr std::string_view doc = R"(The Random
Vectors filter generates a point-centered array of random
vectors. It uses a random number generator to determine
the components of the vectors. This filter operates on any
type of data set, and the output data set will be of the
same type as the input.)";
};

void registervtkBrownianPoints(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkBrownianPoints>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
