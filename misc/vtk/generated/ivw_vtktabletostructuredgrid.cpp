#include "ivw_vtktabletostructuredgrid.h"
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
#include <vtkTableToStructuredGrid.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkTableToStructuredGrid& filter) {
        filter.SetXColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"XColumn", "XColumn",
                            R"(This property specifies which data array is going to be
used as the X coordinate in the generated polydata
dataset.)"_help,
                            ""};
};

struct Wrapper1 {
    bool set(vtkTableToStructuredGrid& filter) {
        filter.SetYColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"YColumn", "YColumn",
                            R"(This property specifies which data array is going to be
used as the Y coordinate in the generated polydata
dataset.)"_help,
                            ""};
};

struct Wrapper2 {
    bool set(vtkTableToStructuredGrid& filter) {
        filter.SetZColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"ZColumn", "ZColumn",
                            R"(This property specifies which data array is going to be
used as the Z coordinate in the generated polydata
dataset.)"_help,
                            ""};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkTableToStructuredGrid> {
    static constexpr std::string_view uri = "vtk.vtkTableToStructuredGrid";
    static constexpr std::string_view className = "vtkTableToStructuredGrid";
    static constexpr std::string_view identifier = "TableToStructuredGrid";
    static constexpr std::string_view displayName = "Table To Structured Grid";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkTable", 1, R"(This property specifies the input..)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2> properties;

    static constexpr std::string_view doc = R"(The
TableToStructuredGrid filter converts a vtkTable to a
vtkStructuredGrid. One must specifies the columns in the
input table to use as the X, Y and Z coordinates for the
points in the output, and the whole
extent.)";
};

void registervtkTableToStructuredGrid(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkTableToStructuredGrid>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
