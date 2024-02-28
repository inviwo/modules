#include "ivw_vtkunstructuredgridghostcellsgenerator.h"
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
#include <vtkUnstructuredGridGhostCellsGenerator.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkUnstructuredGridGhostCellsGenerator& filter) {
        filter.SetBuildIfRequired(property.get());
        return true;
    }
    BoolProperty property{"BuildIfRequired", "BuildIfRequired",
                          R"(Specify if the filter must generate the ghost cells only
if required by the pipeline downstream. To force at least a fixed level
of ghosts, this must be set to 0 (unchecked).)"_help,
                          false};
};

struct Wrapper1 {
    bool set(vtkUnstructuredGridGhostCellsGenerator& filter) {
        filter.SetMinimumNumberOfGhostLevels(property.get());
        return true;
    }
    IntProperty property{
        "MinimumNumberOfGhostLevels",
        "MinimumNumberOfGhostLevels",
        R"(When **BuildIfRequired** if off, use this to specify the minimum number of
ghost cells to request. The filter may request more ghost levels than indicated if a
downstream filter asked for more ghost levels.)"_help,
        1,
        std::pair{1, ConstraintBehavior::Ignore},
        std::pair{10, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkUnstructuredGridGhostCellsGenerator& filter) {
        filter.SetUseGlobalPointIds(property.get());
        return true;
    }
    BoolProperty property{"UseGlobalIds", "UseGlobalIds",
                          R"(Specify if the filter must take benefit of global point
ids if they exist or if point coordinates should be used instead.)"_help,
                          true};
};

struct Wrapper3 {
    bool set(vtkUnstructuredGridGhostCellsGenerator& filter) {
        filter.SetGlobalPointIdsArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"GlobalPointIdsArrayName", "GlobalPointIdsArrayName",
                            R"(This property provides the name for the input array
containing the global point ids if the GlobalIds array of the point
data if not set. Default is GlobalNodeIds.)"_help,
                            "GlobalNodeIds"};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkUnstructuredGridGhostCellsGenerator> {
    static constexpr std::string_view uri = "vtk.vtkUnstructuredGridGhostCellsGenerator";
    static constexpr std::string_view className = "vtkUnstructuredGridGhostCellsGenerator";
    static constexpr std::string_view identifier = "GhostCellsGenerator";
    static constexpr std::string_view displayName = "Ghost Cells Generator";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkUnstructuredGrid", -1, R"(This property specifies the input to the ghost cells
generator.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;
    static constexpr std::string_view doc =
        R"(The GhostCellGenerator operates on unstructured grids only.
This filter does not redistribute the input data, it only
generates ghost cells at processor boundaries by fetching
topological and geometrical information of those cells on
neighbor ranks. The filter can take advantage of global point
ids if they are available - if so it will perform faster,
otherwise point coordinates will be exchanged and processed.)";
};

void registervtkUnstructuredGridGhostCellsGenerator(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkUnstructuredGridGhostCellsGenerator>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
