#include "ivw_vtksimplebondperceiver.h"
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
#include <vtkSimpleBondPerceiver.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkSimpleBondPerceiver& filter) {
        filter.SetTolerance(property.get());
        return true;
    }
    DoubleProperty property{
        "Tolerance",
        "Tolerance",
        R"(This property determines the tolerance to apply on covalent radius.)"_help,
        0.45,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkSimpleBondPerceiver& filter) {
        filter.SetIsToleranceAbsolute(property.get());
        return true;
    }
    BoolProperty property{
        "IsToleranceAbsolute", "IsToleranceAbsolute",
        R"(This property determines if the tolerance is absolute (value is added to radius and should be positive)
or not (value multiplied with radius and should be greater than 1).)"_help,
        true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkSimpleBondPerceiver> {
    static constexpr std::string_view uri = "vtk.vtkSimpleBondPerceiver";
    static constexpr std::string_view className = "vtkSimpleBondPerceiver";
    static constexpr std::string_view identifier = "ComputeMoleculeBonds";
    static constexpr std::string_view displayName = "Compute Molecule Bonds";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkMolecule", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;
    static constexpr std::string_view doc = R"(Compute the bonds of a molecule. If the
interatomic distance is less than the sum of the two atom's covalent radii
(and a tolerance), a single bond is added.
This algorithm does not consider valences, hybridization, aromaticity, or
anything other than atomic separations. It will not produce anything other
than single bonds.)";
};

void registervtkSimpleBondPerceiver(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkSimpleBondPerceiver>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
