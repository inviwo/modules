#include "ivw_vtkwarpvector.h"
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
#include <vtkWarpVector.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkWarpVector& filter) {
        if (name.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, fieldAssociation.get(), name.get().c_str());
        return true;
    }
    OptionPropertyString name{"name", "Name", {}, 0};

    OptionProperty<vtkDataObject::FieldAssociations> fieldAssociation{
        "fieldAssociation",
        "Field Association",
        {{"points", "Points", vtkDataObject::FIELD_ASSOCIATION_POINTS},
         {"cells", "Cells", vtkDataObject::FIELD_ASSOCIATION_CELLS},
         {"none", "None", vtkDataObject::FIELD_ASSOCIATION_NONE},
         {"pointsThenCells", "Points then Cells",
          vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS}},
        3};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{"SelectInputVectors", "Vectors",
                              R"(The value of this property contains the name of the
vector array by which to warp the dataset's point
coordinates.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkWarpVector& filter) {
        filter.SetScaleFactor(property.get());
        return true;
    }
    DoubleProperty property{"ScaleFactor",
                            "ScaleFactor",
                            R"(Each component of the selected vector array will be
multiplied by the value of this property before being used to compute
new point coordinates.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkWarpVector> {
    static constexpr std::string_view uri = "vtk.vtkWarpVector";
    static constexpr std::string_view className = "vtkWarpVector";
    static constexpr std::string_view identifier = "WarpVector";
    static constexpr std::string_view displayName = "Warp By Vector";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkPointSet", 3, R"(This property specifies the input to the Warp (vector)
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;

    static constexpr std::string_view doc =
        R"ivw(The Warp (vector) filter translates the points of the
input dataset using a specified vector array. The vector
array chosen specifies a vector per point in the input.
Each point is translated along its vector by a given scale
factor. This filter operates on polygonal, curvilinear,
and unstructured grid datasets. Because this filter only
changes the positions of the points, the output dataset
type is the same as that of the input.)ivw";
};

void registervtkWarpVector(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkWarpVector>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
