#include "ivw_vtkgaussiansplatter.h"
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
#include <vtkGaussianSplatter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkGaussianSplatter& filter) {
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
        CompositeProperty tmp{"SelectInputScalars", "Resample Field",
                              R"(Choose a scalar array to splat into the output cells. If
ignore arrays is chosen, point density will be counted
instead.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkGaussianSplatter& filter) {
        filter.SetSampleDimensions(property.get(0), property.get(1), property.get(2));
        return true;
    }
    IntVec3Property property{"SampleDimensions",
                             "Resampling Grid",
                             R"(Set / get the dimensions of the sampling structured
point set. Higher values produce better results but are much
slower.)"_help,
                             ivec3{50, 50, 50},
                             std::pair{ivec3{0, 0, 0}, ConstraintBehavior::Ignore},
                             std::pair{ivec3{100, 100, 100}, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkGaussianSplatter& filter) {
        filter.SetRadius(property.get());
        return true;
    }
    DoubleProperty property{"Radius",
                            "Gaussian Splat Radius",
                            R"(Set / get the radius of propagation of the splat. This
value is expressed as a percentage of the length of the longest side of
the sampling volume. Smaller numbers greatly reduce execution
time.)"_help,
                            0.1,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkGaussianSplatter& filter) {
        filter.SetExponentFactor(property.get());
        return true;
    }
    DoubleProperty property{"ExponentFactor",
                            "Gaussian Exponent Factor",
                            R"(Set / get the sharpness of decay of the splats. This is
the exponent constant in the Gaussian equation. Normally this is a
negative value.)"_help,
                            -5.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{0.0, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(vtkGaussianSplatter& filter) {
        filter.SetScalarWarping(property.get());
        return true;
    }
    BoolProperty property{"ScalarWarping", "Scale Splats",
                          R"(Turn on/off the scaling of splats by scalar
value.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(vtkGaussianSplatter& filter) {
        filter.SetScaleFactor(property.get());
        return true;
    }
    DoubleProperty property{"ScaleFactor",
                            "Scale Factor",
                            R"(Multiply Gaussian splat distribution by this value. If
ScalarWarping is on, then the Scalar value will be multiplied by the
ScaleFactor times the Gaussian function.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(vtkGaussianSplatter& filter) {
        filter.SetNormalWarping(property.get());
        return true;
    }
    BoolProperty property{"NormalWarping", "Elliptical Splats",
                          R"(Turn on/off the generation of elliptical splats. If
normal warping is on, then the input normals affect the distribution of
the splat. This boolean is used in combination with the Eccentricity
ivar.)"_help,
                          true};
};

struct Wrapper7 {
    bool set(vtkGaussianSplatter& filter) {
        filter.SetEccentricity(property.get());
        return true;
    }
    DoubleProperty property{"Eccentricity",
                            "Ellipitical Eccentricity",
                            R"(Control the shape of elliptical splatting. Eccentricity
is the ratio of the major axis (aligned along normal) to the minor
(axes) aligned along other two axes. So Eccentricity gt 1 creates
needles with the long axis in the direction of the normal; Eccentricity
lt 1 creates pancakes perpendicular to the normal
vector.)"_help,
                            2.5,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(vtkGaussianSplatter& filter) {
        filter.SetCapping(property.get());
        return true;
    }
    BoolProperty property{"Capping", "Fill Volume Boundary",
                          R"(Turn on/off the capping of the outer boundary of the
volume to a specified cap value. This can be used to close surfaces
(after isosurfacing) and create other effects.)"_help,
                          true};
};

struct Wrapper9 {
    bool set(vtkGaussianSplatter& filter) {
        filter.SetCapValue(property.get());
        return true;
    }
    DoubleProperty property{"CapValue",
                            "Fill Value",
                            R"(Specify the cap value to use. (This instance variable
only has effect if the ivar Capping is on.))"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(vtkGaussianSplatter& filter) {
        filter.SetAccumulationMode(property.get());
        return true;
    }
    OptionPropertyInt property{"AccumulationMode",
                               "Splat Accumulation Mode",
                               R"(Specify the scalar accumulation mode. This mode
expresses how scalar values are combined when splats are overlapped.
The Max mode acts like a set union operation and is the most commonly
used; the Min mode acts like a set intersection, and the sum is just
weird.)"_help,
                               {{"Min", "Min", 0}, {"Max", "Max", 1}, {"Sum", "Sum", 2}},
                               1};
};

struct Wrapper11 {
    bool set(vtkGaussianSplatter& filter) {
        filter.SetNullValue(property.get());
        return true;
    }
    DoubleProperty property{"NullValue",
                            "Empty Cell Value",
                            R"(Set the Null value for output points not receiving a
contribution from the input points. (This is the initial value of the
voxel samples.))"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkGaussianSplatter> {
    static constexpr std::string_view uri = "vtk.vtkGaussianSplatter";
    static constexpr std::string_view className = "vtkGaussianSplatter";
    static constexpr std::string_view identifier = "GaussianSplatter";
    static constexpr std::string_view displayName = "Gaussian Resampling";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", 1, R"(This property specifies the input to the
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11>
        properties;
    static constexpr std::string_view doc = R"(vtkGaussianSplatter
is a filter that injects input points into a structured
points (volume) dataset. As each point is injected, it
"splats" or distributes values to nearby voxels. Data is
distributed using an elliptical, Gaussian distribution
function. The distribution function is modified using
scalar values (expands distribution) or normals (creates
ellipsoidal distribution rather than spherical). Warning:
results may be incorrect in parallel as points can't splat
into other processor's cells.)";
};

void registervtkGaussianSplatter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkGaussianSplatter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
