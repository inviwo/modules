#include "ivw_vtkwarpscalar.h"
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
#include <vtkWarpScalar.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkWarpScalar& filter) {
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
        CompositeProperty tmp{"SelectInputScalars", "Scalars",
                              R"(This property contains the name of the scalar array by
which to warp the dataset.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkWarpScalar& filter) {
        filter.SetScaleFactor(property.get());
        return true;
    }
    DoubleProperty property{"ScaleFactor",
                            "ScaleFactor",
                            R"(The scalar value at a given point is multiplied by the
value of this property to determine the magnitude of the change vector
for that point.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkWarpScalar& filter) {
        filter.SetNormal(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{"Normal",
                                "Normal",
                                R"(The values of this property specify the direction along
which to warp the dataset if any normals contained in the input dataset
are not being used for this purpose. (See the UseNormal
property.))"_help,
                                dvec3{0.0, 0.0, 1.0},
                                std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkWarpScalar& filter) {
        filter.SetUseNormal(property.get());
        return true;
    }
    BoolProperty property{"UseNormal", "UseNormal",
                          R"(If point normals are present in the dataset, the value
of this property toggles whether to use a single normal value (value =
1) or the normals from the dataset (value = 0).)"_help,
                          false};
};

struct Wrapper4 {
    bool set(vtkWarpScalar& filter) {
        filter.SetXYPlane(property.get());
        return true;
    }
    BoolProperty property{"XYPlane", "XY Plane", R"(If the value of this property is 1, then the
Z-coordinates from the input are considered to be the scalar values,
and the displacement is along the Z axis. This is useful for creating
carpet plots.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkWarpScalar> {
    static constexpr std::string_view uri = "vtk.vtkWarpScalar";
    static constexpr std::string_view className = "vtkWarpScalar";
    static constexpr std::string_view identifier = "WarpScalar";
    static constexpr std::string_view displayName = "Warp By Scalar";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkPointSet", 1, R"(This property specifies the input to the Warp (scalar)
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;
    static constexpr std::string_view doc =
        R"(The Warp (scalar) filter translates the points of the
input data set along a vector by a distance determined by
the specified scalars. This filter operates on polygonal,
curvilinear, and unstructured grid data sets containing
single-component scalar arrays. Because it only changes
the positions of the points, the output data set type is
the same as that of the input. Any scalars in the input
dataset are copied to the output, so the data can be
colored by them.)";
};

void registervtkWarpScalar(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkWarpScalar>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
