#include "ivw_vtkmoleculeappend.h"

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
#include <vtkMoleculeAppend.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkMoleculeAppend> {
    static constexpr std::string_view className = "vtkMoleculeAppend";
    static constexpr std::string_view identifier = "AppendMolecule";
    static constexpr std::string_view displayName = "Append Molecule";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkMolecule", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;
    static constexpr std::string_view doc =
        R"(Appends one or more molecules into a single molecule. It also appends  the associated atom data and edge data.
Note that input data arrays should match (same number of arrays with same names in each input))";
};

void registervtkMoleculeAppend(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkMoleculeAppend>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
