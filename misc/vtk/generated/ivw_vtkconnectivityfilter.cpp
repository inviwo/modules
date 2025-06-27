#include "ivw_vtkconnectivityfilter.h"
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
#include <vtkConnectivityFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkConnectivityFilter& filter) {
        filter.SetExtractionMode(property.get());
        return true;
    }
    OptionPropertyInt property{
        "ExtractionMode",
        "ExtractionMode",
        R"(Controls the extraction of connected
surfaces.)"_help,
        {{"Extract Largest Region", "Extract Largest Region", 4},
         {"Extract All Regions", "Extract All Regions", 5},
         {"Extract Closest Point Region", "Extract Closest Point Region", 6}},
        1};
};

struct Wrapper1 {
    bool set(vtkConnectivityFilter& filter) {
        filter.SetColorRegions(property.get());
        return true;
    }
    BoolProperty property{"ColorRegions", "ColorRegions", R"(Controls the coloring of the connected
regions.)"_help,
                          true};
};

struct Wrapper2 {
    bool set(vtkConnectivityFilter& filter) {
        filter.SetRegionIdAssignmentMode(property.get());
        return true;
    }
    OptionPropertyInt property{
        "RegionIdAssignmentMode",
        "RegionIdAssignmentMode",
        R"(Specifies how regions IDs are assigned to the connected components. **Unspecified** means
regions will have no particular order, **Cell Count Descending** assigns increasing region
IDs to connected components with progressively smaller cell counts, and **Cell Count Ascending**
assigns increasing region IDs to connected components with progressively larger cell counts.)"_help,
        {{"Unspecified", "Unspecified", 0},
         {"Cell Count Descending", "Cell Count Descending", 1},
         {"Cell Count Ascending", "Cell Count Ascending", 2}},
        0};
};

struct Wrapper3 {
    bool set(vtkConnectivityFilter& filter) {
        filter.SetClosestPoint(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{"ClosestPoint",
                                "ClosestPoint",
                                R"(Specifies the point to use in closest point mode.)"_help,
                                dvec3{0.0, 0.0, 0.0},
                                std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkConnectivityFilter> {
    static constexpr std::string_view uri = "vtk.vtkConnectivityFilter";
    static constexpr std::string_view className = "vtkConnectivityFilter";
    static constexpr std::string_view identifier = "PVConnectivityFilter";
    static constexpr std::string_view displayName = "Connectivity";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataSet", -1, R"(This property specifies the input to the Connectivity
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;

    static constexpr std::string_view doc = R"ivw(The Connectivity
filter assigns a region id to connected components of the
input data set. (The region id is assigned as a point
scalar value.) This filter takes any data set type as
input and produces unstructured grid
output.)ivw";
};

void registervtkConnectivityFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkConnectivityFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
