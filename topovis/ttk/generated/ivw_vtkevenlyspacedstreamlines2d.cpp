#include "ivw_vtkevenlyspacedstreamlines2d.h"

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
#include <vtkEvenlySpacedStreamlines2D.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkEvenlySpacedStreamlines2D& filter) {
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
    bool set(vtkEvenlySpacedStreamlines2D& filter) {
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
    bool set(vtkEvenlySpacedStreamlines2D& filter) {
        filter.SetIntegratorType(property.get());
        return true;
    }
    OptionPropertyInt property{
        "IntegratorType",
        "IntegratorType",
        R"(This property determines which integrator (with
increasing accuracy) to use for creating streamlines.)"_help,
        {{"Runge-Kutta 2", "Runge-Kutta 2", 0}, {"Runge-Kutta 4", "Runge-Kutta 4", 1}},
        1};
};

struct Wrapper3 {
    bool set(vtkEvenlySpacedStreamlines2D& filter) {
        filter.SetIntegrationStepUnit(property.get());
        return true;
    }
    OptionPropertyInt property{"IntegrationStepUnit",
                               "Integration Step Unit",
                               R"(This property specifies the unit for
Initial integration step size. The Length unit refers
to the arc length that a particle travels/advects within a single step.
The Cell Length unit represents the step size as a number of
cells.)"_help,
                               {{"Length", "Length", 1}, {"Cell Length", "Cell Length", 2}},
                               1};
};

struct Wrapper4 {
    bool set(vtkEvenlySpacedStreamlines2D& filter) {
        filter.SetInitialIntegrationStep(property.get());
        return true;
    }
    DoubleProperty property{"InitialIntegrationStep",
                            "Initial Step Length",
                            R"(This property specifies the initial integration step
size. For non-adaptive integrators (Runge-Kutta 2 and Runge-Kutta 4),
it is fixed (always equal to this initial value) throughout the
integration.)"_help,
                            0.2,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(vtkEvenlySpacedStreamlines2D& filter) {
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

struct Wrapper6 {
    bool set(vtkEvenlySpacedStreamlines2D& filter) {
        filter.SetSeparatingDistance(property.get());
        return true;
    }
    DoubleProperty property{"SeparatingDistance",
                            "Separating Distance",
                            R"(Specify the separating distance between
streamlines expressed in IntegrationStepUnit.)"_help,
                            2.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(vtkEvenlySpacedStreamlines2D& filter) {
        filter.SetSeparatingDistanceRatio(property.get());
        return true;
    }
    DoubleProperty property{"SeparatingDistanceRatio",
                            "Separating Distance Ratio",
                            R"(Specifies SeparatingDistanceRatio. If streamlines
get closer than SeparatingDistance * SeparatingDistanceRatio to
other streamlines integration stops.)"_help,
                            0.4,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(vtkEvenlySpacedStreamlines2D& filter) {
        filter.SetClosedLoopMaximumDistance(property.get());
        return true;
    }
    DoubleProperty property{"ClosedLoopMaximumDistance",
                            "Closed Loop Maximum Distance",
                            R"(Loops are considered closed if the have two points at
distance less than this. This is expressed in IntegrationStepUnit.)"_help,
                            0.2,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(vtkEvenlySpacedStreamlines2D& filter) {
        filter.SetStartPosition(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{"StartPosition",
                                "Start Position",
                                R"(Specify the starting point (seed) of the first streamline
in the global coordinate system.)"_help,
                                dvec3{0.0, 0.0, 0.0},
                                std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(vtkEvenlySpacedStreamlines2D& filter) {
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

struct Wrapper11 {
    bool set(vtkEvenlySpacedStreamlines2D& filter) {
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
struct TTKTraits<vtkEvenlySpacedStreamlines2D> {
    static constexpr std::string_view className = "vtkEvenlySpacedStreamlines2D";
    static constexpr std::string_view identifier = "EvenlySpacedStreamlines2D";
    static constexpr std::string_view displayName = "Evenly Spaced Streamlines 2D";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", 3, R"(This property specifies the input to the filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {
        Group{"Streamline Parameters",
              {"StartPosition", "SeparatingDistance", "SeparatingDistanceRatio",
               "ClosedLoopMaximumDistance", "IntegratorType", "IntegrationStepUnit",
               "InitialIntegrationStep", "MaximumNumberOfSteps", "TerminalSpeed"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11>
        properties;
    static constexpr std::string_view doc =
        R"(This filter generates evenly spaced streamlines in a 2D
vector field from a start position. Production of
streamlines terminates if a streamline crosses the
exterior boundary of the input dataset
(ReasonForTermination=1), an initialization issue (ReasonForTermination=2),
computing an unexpected value (ReasonForTermination=3),
reached the Maximum Streamline Length input value
(ReasonForTermination=4), reached the Maximum Steps
input value (ReasonForTermination=5), velocity was
lower than the Terminal Speed input value
(ReasonForTermination=6), a streamline formed a loop
(ReasonForTermination=7), and the streamline was too close to
other streamlines (ReasonForTermination=8). This filter
operates on a 2D dataset aligned with plane XY with
point-centered vectors aligned with plane XY.
The output is polygonal data containing polylines.)";
};

void registervtkEvenlySpacedStreamlines2D(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkEvenlySpacedStreamlines2D>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
