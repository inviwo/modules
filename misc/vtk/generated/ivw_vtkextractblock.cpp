#include "ivw_vtkextractblock.h"
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
#include <vtkExtractBlock.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkExtractBlock& filter) {
        filter.SetPruneOutput(property.get());
        return true;
    }
    BoolProperty property{"PruneOutput", "PruneOutput",
                          R"(When set, the output multiblock dataset will be pruned
to remove empty nodes. On by default.)"_help,
                          true};
};

struct Wrapper1 {
    bool set(vtkExtractBlock& filter) {
        filter.SetMaintainStructure(property.get());
        return true;
    }
    BoolProperty property{"MaintainStructure", "MaintainStructure",
                          R"(This is used only when PruneOutput is ON. By default,
when pruning the output i.e. remove empty blocks, if node has only 1
non-null child block, then that node is removed. To preserve these
parent nodes, set this flag to true.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkExtractBlock> {
    static constexpr std::string_view uri = "vtk.vtkExtractBlock";
    static constexpr std::string_view className = "vtkExtractBlock";
    static constexpr std::string_view identifier = "ExtractBlock";
    static constexpr std::string_view displayName = "Extract Block";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkMultiBlockDataSet", -1,
                  R"(This property specifies the input to the Extract Group
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;

    static constexpr std::string_view doc = R"(This filter extracts a range
of groups from a multiblock dataset)";
};

void registervtkExtractBlock(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkExtractBlock>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
