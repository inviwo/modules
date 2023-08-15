#include "ivw_vtkcontourfilter.h"

#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/ttk/processors/ttkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include "vtkContourFilter.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkContourFilter& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"SelectInputScalars", "Contour By", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkContourFilter& filter) {
        filter.SetComputeNormals(property.get());
        return true;
    }
    BoolProperty property{"ComputeNormals", "ComputeNormals", true};
};

struct Wrapper2 {
    bool set(vtkContourFilter& filter) {
        filter.SetComputeGradients(property.get());
        return true;
    }
    BoolProperty property{"ComputeGradients", "ComputeGradients", false};
};

struct Wrapper3 {
    bool set(vtkContourFilter& filter) {
        filter.SetComputeScalars(property.get());
        return true;
    }
    BoolProperty property{"ComputeScalars", "ComputeScalars", true};
};

struct Wrapper4 {
    bool set(vtkContourFilter& filter) {
        filter.SetOutputPointsPrecision(property.get());
        return true;
    }
    OptionPropertyInt property{
        "OutputPointsPrecision",
        "OutputPointsPrecision",
        {{"Single", "Single", 0}, {"Double", "Double", 1}, {"Same as input", "Same as input", 2}},
        2};
};

struct Wrapper5 {
    bool set(vtkContourFilter& filter) {
        filter.SetGenerateTriangles(property.get());
        return true;
    }
    BoolProperty property{"GenerateTriangles", "GenerateTriangles", true};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkContourFilter> {
    static constexpr std::string_view identifier = "Contour";
    static constexpr std::string_view displayName = "Contour";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {Group{"Isosurfaces", {"ContourValues"}},
                                                 Group{"Point Locator", {"Locator"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;
};

void registervtkContourFilter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkContourFilter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
