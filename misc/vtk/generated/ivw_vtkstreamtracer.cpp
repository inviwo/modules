#include "ivw_vtkstreamtracer.h"
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
#include <vtkStreamTracer.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkStreamTracer& filter) {
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
                              R"(This property contains the name of the vector array from
which to generate streamlines.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkStreamTracer& filter) {
        filter.SetInterpolatorType(property.get());
        return true;
    }
    OptionPropertyInt property{
        "InterpolatorType",
        "InterpolatorType",
        R"(This property determines which interpolator to use for
evaluating the velocity vector field. The first is faster though the
second is more robust in locating cells during streamline
integration.)"_help,
        {{"Interpolator with Point Locator", "Interpolator with Point Locator", 0},
         {"Interpolator with Cell Locator", "Interpolator with Cell Locator", 1}},
        0};
};

struct Wrapper2 {
    bool set(vtkStreamTracer& filter) {
        filter.SetSurfaceStreamlines(property.get());
        return true;
    }
    BoolProperty property{"SurfaceStreamlines", "Surface Streamlines",
                          R"(Specify whether or not to compute surface
streamlines.)"_help,
                          false};
};

struct Wrapper3 {
    bool set(vtkStreamTracer& filter) {
        filter.SetIntegrationDirection(property.get());
        return true;
    }
    OptionPropertyInt property{
        "IntegrationDirection",
        "IntegrationDirection",
        R"(This property determines in which direction(s) a
streamline is generated.)"_help,
        {{"FORWARD", "FORWARD", 0}, {"BACKWARD", "BACKWARD", 1}, {"BOTH", "BOTH", 2}},
        2};
};

struct Wrapper4 {
    bool set(vtkStreamTracer& filter) {
        filter.SetIntegratorType(property.get());
        return true;
    }
    OptionPropertyInt property{"IntegratorType",
                               "IntegratorType",
                               R"(This property determines which integrator (with
increasing accuracy) to use for creating streamlines.)"_help,
                               {{"Runge-Kutta 2", "Runge-Kutta 2", 0},
                                {"Runge-Kutta 4", "Runge-Kutta 4", 1},
                                {"Runge-Kutta 4-5", "Runge-Kutta 4-5", 2}},
                               2};
};

struct Wrapper5 {
    bool set(vtkStreamTracer& filter) {
        filter.SetIntegrationStepUnit(property.get());
        return true;
    }
    OptionPropertyInt property{"IntegrationStepUnit",
                               "Integration Step Unit",
                               R"(This property specifies the unit for
Minimum/Initial/Maximum integration step size. The Length unit refers
to the arc length that a particle travels/advects within a single step.
The Cell Length unit represents the step size as a number of
cells.)"_help,
                               {{"Length", "Length", 1}, {"Cell Length", "Cell Length", 2}},
                               1};
};

struct Wrapper6 {
    bool set(vtkStreamTracer& filter) {
        filter.SetInitialIntegrationStep(property.get());
        return true;
    }
    DoubleProperty property{"InitialIntegrationStep",
                            "Initial Step Length",
                            R"(This property specifies the initial integration step
size. For non-adaptive integrators (Runge-Kutta 2 and Runge-Kutta 4),
it is fixed (always equal to this initial value) throughout the
integration. For an adaptive integrator (Runge-Kutta 4-5), the actual
step size varies such that the numerical error is less than a specified
threshold.)"_help,
                            0.2,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(vtkStreamTracer& filter) {
        filter.SetMinimumIntegrationStep(property.get());
        return true;
    }
    DoubleProperty property{"MinimumIntegrationStep",
                            "Minimum Step Length",
                            R"(When using the Runge-Kutta 4-5 integrator, this property
specifies the minimum integration step size.)"_help,
                            0.01,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(vtkStreamTracer& filter) {
        filter.SetMaximumIntegrationStep(property.get());
        return true;
    }
    DoubleProperty property{"MaximumIntegrationStep",
                            "Maximum Step Length",
                            R"(When using the Runge-Kutta 4-5 integrator, this property
specifies the maximum integration step size.)"_help,
                            0.5,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(vtkStreamTracer& filter) {
        filter.SetMaximumNumberOfSteps(property.get());
        return true;
    }
    IntProperty property{"MaximumNumberOfSteps",
                         "Maximum Steps",
                         R"(This property specifies the maximum number of steps,
beyond which streamline integration is terminated.)"_help,
                         2000,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(vtkStreamTracer& filter) {
        filter.SetMaximumPropagation(property.get());
        return true;
    }
    DoubleProperty property{"MaximumPropagation",
                            "Maximum Streamline Length",
                            R"(This property specifies the maximum streamline length
(i.e., physical arc length), beyond which line integration is
terminated.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper11 {
    bool set(vtkStreamTracer& filter) {
        filter.SetTerminalSpeed(property.get());
        return true;
    }
    DoubleProperty property{"TerminalSpeed",
                            "Terminal Speed",
                            R"(This property specifies the terminal speed, below which
particle advection/integration is terminated.)"_help,
                            1e-12,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper12 {
    bool set(vtkStreamTracer& filter) {
        filter.SetMaximumError(property.get());
        return true;
    }
    DoubleProperty property{"MaximumError",
                            "MaximumError",
                            R"(This property specifies the maximum error (for
Runge-Kutta 4-5) tolerated throughout streamline integration. The
Runge-Kutta 4-5 integrator tries to adjust the step size such that the
estimated error is less than this threshold.)"_help,
                            1e-06,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper13 {
    bool set(vtkStreamTracer& filter) {
        filter.SetComputeVorticity(property.get());
        return true;
    }
    BoolProperty property{"ComputeVorticity", "ComputeVorticity",
                          R"(Specify whether or not to compute
vorticity.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkStreamTracer> {
    static constexpr std::string_view uri = "vtk.vtkStreamTracer";
    static constexpr std::string_view className = "vtkStreamTracer";
    static constexpr std::string_view identifier = "StreamTracer";
    static constexpr std::string_view displayName = "Stream Tracer";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Input", "vtkDataSet", 3,
                  R"(This property specifies the input to the Stream Tracer
filter.)"},
        InputData{"Source", "", -1, R"(The value of this property determines how the seeds for
the streamlines will be generated.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Seeds", {"Source"}},
        Group{"Integration Parameters",
              {"IntegrationDirection", "IntegratorType", "IntegrationStepUnit",
               "InitialIntegrationStep", "MinimumIntegrationStep", "MaximumIntegrationStep",
               "MaximumError"}},
        Group{"Streamline Parameters",
              {"MaximumNumberOfSteps", "MaximumPropagation", "TerminalSpeed"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13>
        properties;

    static constexpr std::string_view doc = R"(The
Stream Tracer filter generates streamlines in a vector
field from a collection of seed points. Production of
streamlines terminates if a streamline crosses the
exterior boundary of the input dataset
(ReasonForTermination=1). Other reasons for termination
include an initialization issue (ReasonForTermination=2),
computing an unexpected value (ReasonForTermination=3),
reached the Maximum Streamline Length input value
(ReasonForTermination=4), reached the Maximum Steps
input value (ReasonForTermination=5), and velocity was
lower than the Terminal Speed input value
(ReasonForTermination=6). This filter operates on any
type of dataset, provided it has point-centered vectors.
The output is polygonal data containing polylines.)";
};

void registervtkStreamTracer(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkStreamTracer>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
