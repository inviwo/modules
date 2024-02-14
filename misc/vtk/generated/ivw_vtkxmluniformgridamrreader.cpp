#include "ivw_vtkxmluniformgridamrreader.h"
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
#include <vtkXMLUniformGridAMRReader.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkXMLUniformGridAMRReader& filter) {
        filter.SetMaximumLevelsToReadByDefault(property.get());
        return true;
    }
    IntProperty property{"DefaultNumberOfLevels",
                         "DefaultNumberOfLevels",
                         R"(This reader supports demand-driven heavy data reading i.e. downstream
pipeline can request specific blocks from the AMR using
vtkCompositeDataPipeline::UPDATE_COMPOSITE_INDICES() key in
RequestUpdateExtent() pass. However, when down-stream doesn't provide
any specific keys, the default behavior can be setup to read at-most N
levels by default. The number of levels read can be set using this
method.  Set this to 0 to imply no limit. Default is 1.)"_help,
                         1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{10, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkXMLUniformGridAMRReader& filter) {
        filter.SetActiveTimeDataArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"TimeArray", "Time Array",
                            R"(This property sets which field data to use as time arrays to
read. If set to Default, time steps are incremented integer values starting at zero.)"_help,
                            "TimeValue"};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkXMLUniformGridAMRReader> {
    static constexpr std::string_view uri = "vtk.vtkXMLUniformGridAMRReader";
    static constexpr std::string_view className = "vtkXMLUniformGridAMRReader";
    static constexpr std::string_view identifier = "XMLUniformGridAMRReaderCore";
    static constexpr std::string_view displayName = "XML UniformGrid AMR reader";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK,readers";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;
    static constexpr std::string_view doc = R"()";
};

void registervtkXMLUniformGridAMRReader(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkXMLUniformGridAMRReader>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
