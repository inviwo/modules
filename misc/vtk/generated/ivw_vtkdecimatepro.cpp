#include "ivw_vtkdecimatepro.h"
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
#include <vtkDecimatePro.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkDecimatePro& filter) {
        filter.SetTargetReduction(property.get());
        return true;
    }
    DoubleProperty property{"TargetReduction",
                            "TargetReduction",
                            R"(This property specifies the desired reduction in the
total number of polygons in the output dataset. For example, if the
TargetReduction value is 0.9, the Decimate filter will attempt to
produce an output dataset that is 10% the size of the
input.))"_help,
                            0.9,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkDecimatePro& filter) {
        filter.SetPreserveTopology(property.get());
        return true;
    }
    BoolProperty property{"PreserveTopology", "PreserveTopology",
                          R"(If this property is set to 1, decimation will not split
the dataset or produce holes, but it may keep the filter from reaching
the reduction target. If it is set to 0, better reduction can occur
(reaching the reduction target), but holes in the model may be
produced.)"_help,
                          false};
};

struct Wrapper2 {
    bool set(vtkDecimatePro& filter) {
        filter.SetFeatureAngle(property.get());
        return true;
    }
    DoubleProperty property{"FeatureAngle",
                            "FeatureAngle",
                            R"(The value of this property is used in determining where
the data set may be split. If the angle between two adjacent triangles
is greater than or equal to the FeatureAngle value, then their boundary
is considered a feature edge where the dataset can be
split.)"_help,
                            15.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{180.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkDecimatePro& filter) {
        filter.SetBoundaryVertexDeletion(property.get());
        return true;
    }
    BoolProperty property{"BoundaryVertexDeletion", "BoundaryVertexDeletion",
                          R"(If this property is set to 1, then vertices on the
boundary of the dataset can be removed. Setting the value of this
property to 0 preserves the boundary of the dataset, but it may cause
the filter not to reach its reduction target.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkDecimatePro> {
    static constexpr std::string_view uri = "vtk.vtkDecimatePro";
    static constexpr std::string_view className = "vtkDecimatePro";
    static constexpr std::string_view identifier = "DecimatePro";
    static constexpr std::string_view displayName = "Decimate";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkPolyData", -1, R"(This property specifies the input to the Decimate
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;
    static constexpr std::string_view doc =
        R"(The Decimate filter reduces the number of triangles in a
polygonal data set. Because this filter only operates on
triangles, first run the Triangulate filter on a dataset
that contains polygons other than
triangles.)";
};

void registervtkDecimatePro(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkDecimatePro>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
