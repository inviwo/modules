#include "ivw_vtkxmldataobjectwriter.h"
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
#include <vtkXMLDataObjectWriter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
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
        R"(The mode uses for writing the file's data i.e. ascii,
binary, appended binary.)"_help,
        {{"Ascii", "Ascii", 0}, {"Binary", "Binary", 1}, {"Appended", "Appended", 2}},
        2};
};

struct Wrapper1 {
    bool set(vtkXMLDataObjectWriter& filter) {
        filter.SetHeaderType(property.get());
        return true;
    }
    OptionPropertyInt property{"HeaderType",
                               "HeaderType",
                               R"(Set the binary data header word type.  The default is
UInt32. Set to UInt64 when storing arrays requiring 64-bit indexing.)"_help,
                               {{"UInt32", "UInt32", 32}, {"UInt64", "UInt64", 64}},
                               1};
};

struct Wrapper2 {
    bool set(vtkXMLDataObjectWriter& filter) {
        filter.SetEncodeAppendedData(property.get());
        return true;
    }
    BoolProperty property{"EncodeAppendedData", "EncodeAppendedData",
                          R"(When EncodeAppendedData is turned ON, the writer uses
base64 encoding when writing binary data (only if appended mode is
selected).)"_help,
                          false};
};

struct Wrapper3 {
    bool set(vtkXMLDataObjectWriter& filter) {
        filter.SetCompressorType(property.get());
        return true;
    }
    OptionPropertyInt property{
        "CompressorType",
        "CompressorType",
        R"(The compression algorithm used to compress binary data
(appended mode only). None offers fastest write, LZ4 offers fastest
compressed write, ZLib offers balanced performance, LZMA offers
smallest compressed filesize.)"_help,
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
                               R"(A compression performance parameter from 1 (faster/larger
compressed filesize) to 9 (slower/smaller compressed filesize).)"_help,
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
struct VTKTraits<vtkXMLDataObjectWriter> {
    static constexpr std::string_view uri = "vtk.vtkXMLDataObjectWriter";
    static constexpr std::string_view className = "vtkXMLDataObjectWriter";
    static constexpr std::string_view identifier = "XMLDataObjectWriterCore";
    static constexpr std::string_view displayName = "XMLDataObjectWriterCore";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;
    static constexpr std::string_view doc = R"(Internal writer for writing xml-based vtk
files.)";
};

void registervtkXMLDataObjectWriter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkXMLDataObjectWriter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
