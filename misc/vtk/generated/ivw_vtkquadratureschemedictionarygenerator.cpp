#include "ivw_vtkquadratureschemedictionarygenerator.h"
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
#include <vtkQuadratureSchemeDictionaryGenerator.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkQuadratureSchemeDictionaryGenerator> {
    static constexpr std::string_view uri = "vtk.vtkQuadratureSchemeDictionaryGenerator";
    static constexpr std::string_view className = "vtkQuadratureSchemeDictionaryGenerator";
    static constexpr std::string_view identifier = "QuadratureSchemeDictionaryGenerator";
    static constexpr std::string_view displayName = "Generate Quadrature Scheme Dictionary";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkUnstructuredGrid", -1, R"(This property specifies the input of the
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;

    static constexpr std::string_view doc =
        R"(Generate quadrature scheme dictionaries in data sets that do not have
them.)";
};

void registervtkQuadratureSchemeDictionaryGenerator(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkQuadratureSchemeDictionaryGenerator>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
