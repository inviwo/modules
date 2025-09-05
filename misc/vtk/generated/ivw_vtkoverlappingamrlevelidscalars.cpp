#include "ivw_vtkoverlappingamrlevelidscalars.h"
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
#include <vtkOverlappingAMRLevelIdScalars.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkOverlappingAMRLevelIdScalars> {
    static constexpr std::string_view uri = "vtk.vtkOverlappingAMRLevelIdScalars";
    static constexpr std::string_view className = "vtkOverlappingAMRLevelIdScalars";
    static constexpr std::string_view identifier = "OverlappingLevelIdScalars";
    static constexpr std::string_view displayName = "Level Scalars(Overlapping AMR)";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkOverlappingAMR",
                  .numComp = -1,
                  .doc = R"(This property specifies the input to the Level Scalars
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;

    static constexpr std::string_view doc = R"ivw(The Level
Scalars filter uses colors to show levels of a
hierarchical dataset.)ivw";
};

void registervtkOverlappingAMRLevelIdScalars(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkOverlappingAMRLevelIdScalars>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
