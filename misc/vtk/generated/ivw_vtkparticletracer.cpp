#include "ivw_vtkparticletracer.h"
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
#include <vtkParticleTracer.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkParticleTracer& filter) {
        filter.SetStaticSeeds(property.get());
        return true;
    }
    BoolProperty property{"StaticSeeds", "StaticSeeds",
                          R"(If the input seeds are not changing, then this
can be set to 1 to avoid having to do a repeated grid search
that would return the exact same result.)"_help,
                          false};
};

struct Wrapper1 {
    bool set(vtkParticleTracer& filter) {
        filter.SetStaticMesh(property.get());
        return true;
    }
    BoolProperty property{"StaticMesh", "StaticMesh",
                          R"(If the input grid is not changing, then this
can be set to 1 to avoid having to create cell locators for
each update.)"_help,
                          false};
};

struct Wrapper2 {
    bool set(vtkParticleTracer& filter) {
        filter.SetForceReinjectionEveryNSteps(property.get());
        return true;
    }
    IntProperty property{"ForceReinjectionEveryNSteps",
                         "ForceReinjectionEveryNSteps",
                         R"(When animating particles, it is nice to inject new ones
every Nth step to produce a continuous flow. Setting
ForceReinjectionEveryNSteps to a non zero value will cause the particle
source to reinject particles every Nth step even if it is otherwise
unchanged. Note that if the particle source is also animated, this flag
will be redundant as the particles will be reinjected whenever the
source changes anyway)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper3 : FieldSelection {
    bool set(vtkParticleTracer& filter) {
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
        CompositeProperty tmp{"SelectInputVectors", "SelectInputVectors",
                              R"(Specify which vector array should be used for the
integration through that filter.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper4 {
    bool set(vtkParticleTracer& filter) {
        filter.SetComputeVorticity(property.get());
        return true;
    }
    BoolProperty property{"ComputeVorticity", "ComputeVorticity",
                          R"(Compute vorticity and angular rotation of particles as
they progress)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkParticleTracer> {
    static constexpr std::string_view uri = "vtk.vtkParticleTracer";
    static constexpr std::string_view className = "vtkParticleTracer";
    static constexpr std::string_view identifier = "ParticleTracer";
    static constexpr std::string_view displayName = "ParticleTracer";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Input", "vtkDataObject", 3, R"(Specify which is the Input of the StreamTracer
filter.)"},
        InputData{"Source", "vtkDataSet", -1, R"(Specify the seed dataset. Typically from where the
vector field integration should begin. Usually a point/radius or a line
with a given resolution.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;

    static constexpr std::string_view doc =
        R"(The Particle Trace filter generates pathlines in a vector
field from a collection of seed points. The vector field
used is selected from the Vectors menu, so the input data
set is required to have point-centered vectors. The Seed
portion of the interface allows you to select whether the
seed points for this integration lie in a point cloud or
along a line. Depending on which is selected, the
appropriate 3D widget (point or line widget) is displayed
along with traditional user interface controls for
positioning the point cloud or line within the data set.
Instructions for using the 3D widgets and the
corresponding manual controls can be found in section 7.4.
This filter operates on any type of data set, provided it
has point-centered vectors. The output is polygonal data
containing polylines. This filter is available on the
Toolbar.)";
};

void registervtkParticleTracer(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkParticleTracer>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
