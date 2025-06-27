#include "ivw_vtkyoungsmaterialinterface.h"
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
#include <vtkYoungsMaterialInterface.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkYoungsMaterialInterface& filter) {
        filter.SetInverseNormal(property.get());
        return true;
    }
    BoolProperty property{"InverseNormal", "InverseNormal", R"()"_help, false};
};

struct Wrapper1 {
    bool set(vtkYoungsMaterialInterface& filter) {
        filter.SetReverseMaterialOrder(property.get());
        return true;
    }
    BoolProperty property{"ReverseMaterialOrder", "ReverseMaterialOrder", R"()"_help, false};
};

struct Wrapper2 {
    bool set(vtkYoungsMaterialInterface& filter) {
        filter.SetOnionPeel(property.get());
        return true;
    }
    BoolProperty property{"OnionPeel", "OnionPeel", R"()"_help, true};
};

struct Wrapper3 {
    bool set(vtkYoungsMaterialInterface& filter) {
        filter.SetAxisSymetric(property.get());
        return true;
    }
    BoolProperty property{"AxisSymetric", "AxisSymetric", R"()"_help, true};
};

struct Wrapper4 {
    bool set(vtkYoungsMaterialInterface& filter) {
        filter.SetFillMaterial(property.get());
        return true;
    }
    BoolProperty property{"FillMaterial", "FillMaterial", R"()"_help, true};
};

struct Wrapper5 {
    bool set(vtkYoungsMaterialInterface& filter) {
        filter.SetUseFractionAsDistance(property.get());
        return true;
    }
    BoolProperty property{"UseFractionAsDistance", "UseFractionAsDistance", R"()"_help, false};
};

struct Wrapper6 {
    bool set(vtkYoungsMaterialInterface& filter) {
        filter.SetVolumeFractionRange(property.get(0), property.get(1));
        return true;
    }
    DoubleVec2Property property{"VolumeFractionRange",
                                "VolumeFractionRange",
                                R"()"_help,
                                dvec2{0.01, 0.99},
                                std::pair{dvec2{0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec2{1.0, 1.0}, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkYoungsMaterialInterface> {
    static constexpr std::string_view uri = "vtk.vtkYoungsMaterialInterface";
    static constexpr std::string_view className = "vtkYoungsMaterialInterface";
    static constexpr std::string_view identifier = "YoungsMaterialInterface";
    static constexpr std::string_view displayName = "Youngs Material Interface";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkCompositeDataSet", 1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;

    static constexpr std::string_view doc =
        R"ivw(Computes linear material interfaces in 2D or 3D mixed
cells produced by Eulerian or ALE simulation
codes)ivw";
};

void registervtkYoungsMaterialInterface(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkYoungsMaterialInterface>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
