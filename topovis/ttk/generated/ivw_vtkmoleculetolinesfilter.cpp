#include "ivw_vtkmoleculetolinesfilter.h"

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
#include <vtkMoleculeToLinesFilter.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkMoleculeToLinesFilter> {
    static constexpr std::string_view className = "vtkMoleculeToLinesFilter";
    static constexpr std::string_view identifier = "MoleculeToLines";
    static constexpr std::string_view displayName = "Molecule To Lines";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkMolecule", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;
    static constexpr std::string_view doc =
        R"(Convert a molecule into lines. Each atom of the input becomes a point of the output polydata, each bond a line.)";
};

void registervtkMoleculeToLinesFilter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkMoleculeToLinesFilter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
