#include "ivw_ttkcinemawriter.h"
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/vtk/processors/vtkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/ttk/util/ttkprocessorutils.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataObject.h>
#include <ttkCinemaWriter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkCinemaWriter& filter) {
        if (property.get().empty()) return false;
        filter.SetDatabasePath(property.get().string().c_str());
        return true;
    }
    FileProperty property{"DatabasePath", "DatabasePath",
                          R"(Path to a Cinema Spec D database folder.)"_help,
                          std::filesystem::path{""}};
};

struct Wrapper1 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetCompressionLevel(property.get());
        return true;
    }
    IntProperty property{
        "CompressionLevel",
        "CompressionLevel",
        R"(Determines the compression level form 0 (fast + large files) to 9 (slow + small files).)"_help,
        5,
        std::pair{0, ConstraintBehavior::Ignore},
        std::pair{9, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetFormat(property.get());
        return true;
    }
    OptionPropertyInt property{
        "Format",
        "Store as",
        R"(Store input as VTK file, a PNG image or a TTK compressed file.)"_help,
        {{"VTK File", "VTK File", 0},
         {"PNG Image", "PNG Image", 1},
         {"TTK Compression", "TTK Compression", 2}},
        0};
};

struct Wrapper3 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetIterateMultiBlock(property.get());
        return true;
    }
    BoolProperty property{
        "IterateMultiBlock", "IterateMultiBlock",
        R"(If set to true and the input is a 'vtkMultiBlockDataSet' then store blocks individually.)"_help,
        false};
};

struct Wrapper4 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetForwardInput(property.get());
        return true;
    }
    BoolProperty property{
        "ForwardInput", "ForwardInput",
        R"(Controls if the filter returns an empty output or forwards the input as a shallow copy.)"_help,
        true};
};

struct Wrapper5 {
    bool set(ttkCinemaWriter& filter) {
        filter.DeleteDatabase();
        return true;
    }
    ButtonProperty property{"DeleteDatabase", "DeleteDatabase",
                            R"(Delete the database folder. WARNING: NO UNDO)"_help};
};

struct Wrapper6 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper7 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber",
                         "Thread Number",
                         R"(The maximum number of threads.)"_help,
                         1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel",
                         "Debug Level",
                         R"(Debug level.)"_help,
                         3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetCompactTriangulationCacheSize(property.get());
        return true;
    }
    DoubleProperty property{"CompactTriangulationCacheSize",
                            "Cache",
                            R"(Set the cache size for the compact triangulation as a
ratio with respect to the total cluster number.)"_help,
                            0.2,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(ttkCinemaWriter& filter) {
        filter.Modified();
        return true;
    }
    ButtonProperty property{"Debug_Execute", "Execute",
                            R"(Executes the filter with the last applied parameters, which is
handy to re-start pipeline execution from a specific element
without changing parameters.)"_help};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<ttkCinemaWriter> {
    static constexpr std::string_view uri = "ttk.ttkCinemaWriter";
    static constexpr std::string_view className = "ttkCinemaWriter";
    static constexpr std::string_view identifier = "ttkCinemaWriter";
    static constexpr std::string_view displayName = "TTK CinemaWriter";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "", -1,
        R"(Input data product of any type that is going to be stored in the Cinema database.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 4> groups = {
        Group{"Output Options",
              {"DatabasePath", "CompressionLevel", "Format", "IterateMultiBlock", "ForwardInput"}},
        Group{"Commands", {"DeleteDatabase"}},
        Group{"Topological Compression",
              {"Scalar Field", "CompressionType", "Tolerance", "Subdivide", "MaximumError",
               "ZFPTolerance", "ZFPOnly", "UseTopologicalSimplification", "SQMethod"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"(This filter stores a data product in a Cinema database, and then returns the unmodified input as output.

The field data arrays of the input are used to derive a unique key that identifies the data product in the Cinema database. If another product with the same key already exists then the filter will replace the existing data product, otherwise the data product is simply appended to the database. The data product itself is either stored in a VTK file format)";
};

void registerttkCinemaWriter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkCinemaWriter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
