#include "ivw_vtkextractgeometry.h"
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
#include <vtkExtractGeometry.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkExtractGeometry& filter) {
        filter.SetExtractInside(property.get());
        return true;
    }
    OptionPropertyInt property{"ExtractInside",
                               "Extraction Side",
                               R"(This parameter controls whether to extract cells that
are inside or outside the region.)"_help,
                               {{"outside", "outside", 0}, {"inside", "inside", 1}},
                               1};
};

struct Wrapper1 {
    bool set(vtkExtractGeometry& filter) {
        filter.SetExtractOnlyBoundaryCells(property.get());
        return true;
    }
    BoolProperty property{"Extractonlyintersected", "Extract only intersected",
                          R"(This parameter controls whether to extract only cells
that are on the boundary of the region. If this parameter is set, the
Extraction Side parameter is ignored. If Extract Intersected is off,
this parameter has no effect.)"_help,
                          false};
};

struct Wrapper2 {
    bool set(vtkExtractGeometry& filter) {
        filter.SetExtractBoundaryCells(property.get());
        return true;
    }
    BoolProperty property{"Extractintersected", "Extract intersected",
                          R"(This parameter controls whether to extract cells that
are on the boundary of the region.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkExtractGeometry> {
    static constexpr std::string_view uri = "vtk.vtkExtractGeometry";
    static constexpr std::string_view className = "vtkExtractGeometry";
    static constexpr std::string_view identifier = "ExtractGeometry";
    static constexpr std::string_view displayName = "Extract Cells By Region";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(This property specifies the input to the Slice
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2> properties;
    static constexpr std::string_view doc =
        R"(This filter extracts from its input dataset all cells that are either
completely inside or outside of a specified region (implicit function).
On output, the filter generates an unstructured grid. To use this filter
you must specify a region (implicit function). You must also specify
whether to extract cells lying inside or outside of the region. An
option exists to extract cells that are neither inside or outside (i.e.,
boundary).)";
};

void registervtkExtractGeometry(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkExtractGeometry>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
