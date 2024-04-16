#include "ivw_vtkgenericstreamtracer.h"
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
#include <vtkGenericStreamTracer.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkGenericStreamTracer& filter) {
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
    bool set(vtkGenericStreamTracer& filter) {
        filter.SetMaximumPropagation(property.get());
        return true;
    }
    DoubleProperty property{"MaximumPropagation",
                            "MaximumPropagation",
                            R"(Specify the maximum streamline length.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkGenericStreamTracer& filter) {
        filter.SetInitialIntegrationStep(property.get());
        return true;
    }
    DoubleProperty property{"InitialIntegrationStep",
                            "InitialIntegrationStep",
                            R"(Specify the initial integration step.)"_help,
                            0.5,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkGenericStreamTracer& filter) {
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
    bool set(vtkGenericStreamTracer& filter) {
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
    bool set(vtkGenericStreamTracer& filter) {
        filter.SetMaximumError(property.get());
        return true;
    }
    DoubleProperty property{"MaximumError",
                            "MaximumError",
                            R"(Set the maximum error allowed in the integration. The
meaning of this value depends on the integrator chosen.)"_help,
                            1e-06,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(vtkGenericStreamTracer& filter) {
        filter.SetMinimumIntegrationStep(property.get());
        return true;
    }
    DoubleProperty property{"MinimumIntegrationStep",
                            "MinimumIntegrationStep",
                            R"(Specify the minimum integration step.)"_help,
                            0.01,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(vtkGenericStreamTracer& filter) {
        filter.SetIntegrationStepUnit(property.get());
        return true;
    }
    OptionPropertyInt property{
        "IntegrationStepUnit",
        "IntegrationStepUnit",
        R"(Choose the unit to use for the integration
step.)"_help,
        {{"Time", "Time", 0}, {"Length", "Length", 1}, {"Cell Length", "Cell Length", 2}},
        2};
};

struct Wrapper8 {
    bool set(vtkGenericStreamTracer& filter) {
        filter.SetMaximumIntegrationStep(property.get());
        return true;
    }
    DoubleProperty property{"MaximumIntegrationStep",
                            "MaximumIntegrationStep",
                            R"(Specify the maximum integration step.)"_help,
                            0.01,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(vtkGenericStreamTracer& filter) {
        filter.SetMaximumNumberOfSteps(property.get());
        return true;
    }
    IntProperty property{"MaximumNumberOfSteps",
                         "MaximumNumberOfSteps",
                         R"(Specify the maximum number of steps used in the
integration.)"_help,
                         2000,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(vtkGenericStreamTracer& filter) {
        filter.SetTerminalSpeed(property.get());
        return true;
    }
    DoubleProperty property{"TerminalSpeed",
                            "TerminalSpeed",
                            R"(If at any point the speed is below this value, the
integration is terminated.)"_help,
                            1e-12,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkGenericStreamTracer> {
    static constexpr std::string_view uri = "vtk.vtkGenericStreamTracer";
    static constexpr std::string_view className = "vtkGenericStreamTracer";
    static constexpr std::string_view identifier = "GenericStreamTracer";
    static constexpr std::string_view displayName = "Stream Tracer For Generic Datasets";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Input", "vtkGenericDataSet", 3, R"(Set the input to the Generic Stream Tracer
filter.)"},
        InputData{"Source", "", -1, R"(The value of this property determines how the seeds for
the streamlines will be generated.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10>
        properties;

    static constexpr std::string_view doc = R"(The
Generic Stream Tracer filter generates streamlines in a
vector field from a collection of seed points. The vector
field used is selected from the Vectors menu, so the input
data set is required to have point-centered vectors. The
Seed portion of the interface allows you to select whether
the seed points for this integration lie in a point cloud
or along a line. Depending on which is selected, the
appropriate 3D widget (point or line widget) is displayed
along with traditional user interface controls for
positioning the point cloud or line within the data set.
Instructions for using the 3D widgets and the
corresponding manual controls can be found in section 7.4.
The Max. Propagation entry box allows you to specify the
maximum length of the streamlines. From the Max.
Propagation menu, you can select the units to be either
Time (the time a particle would travel with steady flow)
or Length (in the data set's spatial coordinates). The
Init. Step Len. menu and entry specify the initial step
size for integration. (For non-adaptive integrators,
Runge-Kutta 2 and 4, the initial step size is used
throughout the integration.) The menu allows you to
specify the units. Time and Length have the same meaning
as for Max. Propagation. Cell Length specifies the step
length as a number of cells. The Integration Direction
menu determines in which direction(s) the stream trace
will be generated: FORWARD, BACKWARD, or BOTH. The
Integrator Type section of the interface determines which
calculation to use for integration: Runge-Kutta 2,
Runge-Kutta 4, or Runge-Kutta 4-5. If Runge-Kutta 4-5 is
selected, controls are displayed for specifying the
minimum and maximum step length and the maximum error. The
controls for specifying Min. Step Len. and Max. Step Len.
are the same as those for Init. Step Len. The Runge-Kutta
4-5 integrator tries to choose the step size so that the
estimated error is less than the value of the Maximum
Error entry. If the integration takes more than Max. Steps
to complete, if the speed goes below Term. Speed, if Max.
Propagation is reached, or if a boundary of the input data
set is crossed, integration terminates. This filter
operates on any type of data set, provided it has
point-centered vectors. The output is polygonal data
containing polylines.)";
};

void registervtkGenericStreamTracer(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkGenericStreamTracer>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
