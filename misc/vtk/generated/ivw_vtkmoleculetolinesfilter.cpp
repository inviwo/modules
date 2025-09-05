#include "ivw_vtkmoleculetolinesfilter.h"
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
#include <vtkMoleculeToLinesFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkMoleculeToLinesFilter> {
    static constexpr std::string_view uri = "vtk.vtkMoleculeToLinesFilter";
    static constexpr std::string_view className = "vtkMoleculeToLinesFilter";
    static constexpr std::string_view identifier = "MoleculeToLines";
    static constexpr std::string_view displayName = "Molecule To Lines";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input", .dataType = "vtkMolecule", .numComp = -1, .doc = R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;

    static constexpr std::string_view doc =
        R"ivw(Convert a molecule into lines. Each atom of the input becomes a point of the output polydata, each bond a line.)ivw";
};

void registervtkMoleculeToLinesFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkMoleculeToLinesFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
