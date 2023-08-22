#include "ivw_ttkobjwriter.h"

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
#include <ttkOBJWriter.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkOBJWriter& filter) {
        if (property.get().empty()) return false;
        filter.SetFilename(property.get().string().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName",
                          R"(This property specifies the file name for the OBJ writer.)"_help,
                          std::filesystem::path{""}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkOBJWriter> {
    static constexpr std::string_view className = "ttkOBJWriter";
    static constexpr std::string_view identifier = "ttkOBJWriter";
    static constexpr std::string_view displayName = "TTK OBJWriter";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc =
        R"(Export a VTK Unstructured Grid into a Wavefront OBJ file.)";
};

void registerttkOBJWriter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkOBJWriter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
