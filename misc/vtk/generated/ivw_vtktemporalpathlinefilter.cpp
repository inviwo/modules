#include "ivw_vtktemporalpathlinefilter.h"
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
#include <vtkTemporalPathLineFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkTemporalPathLineFilter& filter) {
        filter.SetMaskPoints(property.get());
        return true;
    }
    IntProperty property{"MaskPoints",
                         "MaskPoints",
                         R"(Set the number of particles to track as a ratio of the input.
Example: setting MaskPoints to 10 will track every 10th point.)"_help,
                         100,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkTemporalPathLineFilter& filter) {
        filter.SetMaxTrackLength(property.get());
        return true;
    }
    IntProperty property{"MaxTrackLength",
                         "MaxTrackLength",
                         R"(If the Particles being traced animate for a long time, the trails
or traces will become long and stringy. Setting the
MaxTraceTimeLength will limit how much of the trace is
displayed. Tracks longer then the Max will disappear and the
trace will appear like a snake of fixed length which progresses
as the particle moves.  This length is given with respect to
timesteps.)"_help,
                         25,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkTemporalPathLineFilter& filter) {
        filter.SetMaxStepDistance(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{"MaxStepDistance",
                                "MaxStepDistance",
                                R"(If a particle disappears from one end of a simulation and
reappears on the other side, the track left will be
unrepresentative.  Set a MaxStepDistance{x,y,z} which acts as a
threshold above which if a step occurs larger than the value (for
the dimension), the track will be dropped and restarted after the
step. (ie the part before the wrap around will be dropped and the
newer part kept).)"_help,
                                dvec3{1.0, 1.0, 1.0},
                                std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkTemporalPathLineFilter& filter) {
        filter.SetIdChannelArray(property.get().c_str());
        return true;
    }
    StringProperty property{"IdChannelArray", "IdChannelArray",
                            R"(Specify the name of a scalar array which will be used to fetch
the index of each point. This is necessary only if the particles
change position (Id order) on each time step. The Id can be used
to identify particles at each step and hence track them properly.
If this array is set to "Global or Local IDs", the global point
ids are used if they exist or the point index is otherwise.)"_help,
                            "Global or Local IDs"};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkTemporalPathLineFilter> {
    static constexpr std::string_view uri = "vtk.vtkTemporalPathLineFilter";
    static constexpr std::string_view className = "vtkTemporalPathLineFilter";
    static constexpr std::string_view identifier = "ParticlePathLines";
    static constexpr std::string_view displayName = "Temporal Particles To Pathlines";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Input", "vtkPointSet", -1, R"(The input cells to create pathlines for.)"},
        InputData{"Selection", "vtkDataSet", -1,
                  R"(Set a second input, which is a selection. Particles with the same
Id in the selection as the primary input will be chosen for
pathlines Note that you must have the same IdChannelArray in the
selection as the input)"}};
    inline static std::array<OutputData, 2> outports = {OutputData{"outport0", "Pathlines", 0},
                                                        OutputData{"outport1", "Particles", 1}};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;
    static constexpr std::string_view doc =
        R"(Particle Pathlines takes any dataset as input, it extracts the
point locations of all cells over time to build up a polyline
trail.  The point number (index) is used as the 'key' if the points
are randomly changing their respective order in the points list,
then you should specify a scalar that represents the unique
ID. This is intended to handle the output of a filter such as the
TemporalStreamTracer.)";
};

void registervtkTemporalPathLineFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkTemporalPathLineFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
