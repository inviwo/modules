#include "ivw_vtkxmldataobjectwriter.h"

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
#include "vtkXMLDataObjectWriter.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkXMLDataObjectWriter& filter) {
        filter.SetDataMode(property.get());
        return true;
    }
    OptionPropertyInt property{
        "DataMode",
        "DataMode",
        {{"Ascii", "Ascii", 0}, {"Binary", "Binary", 1}, {"Appended", "Appended", 2}},
        2};
};

struct Wrapper1 {
    bool set(vtkXMLDataObjectWriter& filter) {
        filter.SetHeaderType(property.get());
        return true;
    }
    OptionPropertyInt property{
        "HeaderType", "HeaderType", {{"UInt32", "UInt32", 32}, {"UInt64", "UInt64", 64}}, 1};
};

struct Wrapper2 {
    bool set(vtkXMLDataObjectWriter& filter) {
        filter.SetEncodeAppendedData(property.get());
        return true;
    }
    BoolProperty property{"EncodeAppendedData", "EncodeAppendedData", false};
};

struct Wrapper3 {
    bool set(vtkXMLDataObjectWriter& filter) {
        filter.SetCompressorType(property.get());
        return true;
    }
    OptionPropertyInt property{
        "CompressorType",
        "CompressorType",
        {{"None", "None", 0}, {"LZ4", "LZ4", 2}, {"ZLib", "ZLib", 1}, {"LZMA", "LZMA", 3}},
        0};
};

struct Wrapper4 {
    bool set(vtkXMLDataObjectWriter& filter) {
        filter.SetCompressionLevel(property.get());
        return true;
    }
    OptionPropertyInt property{"CompressionLevel",
                               "CompressionLevel",
                               {{"1", "1", 1},
                                {"2", "2", 2},
                                {"3", "3", 3},
                                {"4", "4", 4},
                                {"5", "5", 5},
                                {"6", "6", 6},
                                {"7", "7", 7},
                                {"8", "8", 8},
                                {"9", "9", 9}},
                               5};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkXMLDataObjectWriter> {
    static constexpr std::string_view identifier = "XMLDataObjectWriterCore";
    static constexpr std::string_view displayName = "XMLDataObjectWriterCore";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;
};

void registervtkXMLDataObjectWriter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkXMLDataObjectWriter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
