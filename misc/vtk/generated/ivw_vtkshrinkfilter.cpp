#include "ivw_vtkshrinkfilter.h"
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
#include <vtkShrinkFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkShrinkFilter& filter) {
        filter.SetShrinkFactor(property.get());
        return true;
    }
    DoubleProperty property{"ShrinkFactor",
                            "ShrinkFactor",
                            R"(The value of this property determines how far the points
will move. A value of 0 positions the points at the centroid of the
cell; a value of 1 leaves them at their original
positions.)"_help,
                            0.5,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkShrinkFilter> {
    static constexpr std::string_view uri = "vtk.vtkShrinkFilter";
    static constexpr std::string_view className = "vtkShrinkFilter";
    static constexpr std::string_view identifier = "ShrinkFilter";
    static constexpr std::string_view displayName = "Shrink";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(This property specifies the input to the Shrink
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;

    static constexpr std::string_view doc = R"ivw(The Shrink filter
causes the individual cells of a dataset to break apart
from each other by moving each cell's points toward the
centroid of the cell. (The centroid of a cell is the
average position of its points.) This filter operates on
any type of dataset and produces unstructured grid
output.)ivw";
};

void registervtkShrinkFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkShrinkFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
