#include "ivw_vtkidfilter.h"
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
#include <vtkIdFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkIdFilter& filter) {
        filter.SetPointIds(property.get());
        return true;
    }
    BoolProperty property{"GeneratePointIds", "GeneratePointIds",
                          R"(Enable generation of point ids.)"_help, true};
};

struct Wrapper1 {
    bool set(vtkIdFilter& filter) {
        filter.SetPointIdsArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"PointIdsArrayName", "PointIdsArrayName",
                            R"(The name of the point ids array.)"_help, "PointIds"};
};

struct Wrapper2 {
    bool set(vtkIdFilter& filter) {
        filter.SetCellIds(property.get());
        return true;
    }
    BoolProperty property{"GenerateCellIds", "GenerateCellIds",
                          R"(Enable generation of cell ids.)"_help, true};
};

struct Wrapper3 {
    bool set(vtkIdFilter& filter) {
        filter.SetCellIdsArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"CellIdsArrayName", "CellIdsArrayName",
                            R"(The name of the cell ids array.)"_help, "CellIds"};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkIdFilter> {
    static constexpr std::string_view uri = "vtk.vtkIdFilter";
    static constexpr std::string_view className = "vtkIdFilter";
    static constexpr std::string_view identifier = "GenerateIdScalars";
    static constexpr std::string_view displayName = "Generate Ids";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkDataSet",
                  .numComp = -1,
                  .doc = R"(This property specifies the input to the Cell Data to
Point Data filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Point Ids", {"GeneratePointIds", "PointIdsArrayName"}},
        Group{"Cell Ids", {"GenerateCellIds", "CellIdsArrayName"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;

    static constexpr std::string_view doc =
        R"ivw(This filter generates scalars using cell and point ids.
That is, the point attribute data scalars are generated
from the point ids, and the cell attribute data scalars or
field data are generated from the the cell
ids.)ivw";
};

void registervtkIdFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkIdFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
