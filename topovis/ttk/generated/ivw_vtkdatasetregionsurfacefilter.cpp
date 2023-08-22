#include "ivw_vtkdatasetregionsurfacefilter.h"

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
#include <vtkDataSetRegionSurfaceFilter.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkDataSetRegionSurfaceFilter& filter) {
        filter.SetPieceInvariant(property.get());
        return true;
    }
    BoolProperty property{"PieceInvariant", "PieceInvariant",
                          R"(If the value of this property is set to 1, internal
surfaces along process boundaries will be removed. NOTE: Enabling this
option might cause multiple executions of the data source because more
information is needed to remove internal surfaces.)"_help,
                          true};
};

struct Wrapper1 {
    bool set(vtkDataSetRegionSurfaceFilter& filter) {
        filter.SetNonlinearSubdivisionLevel(property.get());
        return true;
    }
    IntProperty property{"NonlinearSubdivisionLevel",
                         "NonlinearSubdivisionLevel",
                         R"(If the input is an unstructured grid with nonlinear
faces, this parameter determines how many times the face is subdivided
into linear faces. If 0, the output is the equivalent of its linear
counterpart (and the midpoints determining the nonlinear interpolation
are discarded). If 1, the nonlinear face is triangulated based on the
midpoints. If greater than 1, the triangulated pieces are recursively
subdivided to reach the desired subdivision. Setting the value to
greater than 1 may cause some point data to not be passed even if no
quadratic faces exist. This option has no effect if the input is not an
unstructured grid.)"_help,
                         1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{4, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkDataSetRegionSurfaceFilter& filter) {
        filter.SetRegionArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"RegionArrayName", "RegionArrayName",
                            R"(This property specifies the name of the material
array for generating parts.)"_help,
                            "material"};
};

struct Wrapper3 {
    bool set(vtkDataSetRegionSurfaceFilter& filter) {
        filter.SetSingleSided(property.get());
        return true;
    }
    BoolProperty property{"SingleSided", "SingleSided",
                          R"(If the value of this property is set to 1 (the default),
surfaces along the boundary are 1 layer thick. Otherwise there is
a surface for the material on each side.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(vtkDataSetRegionSurfaceFilter& filter) {
        filter.SetMaterialPropertiesName(property.get().c_str());
        return true;
    }
    StringProperty property{"MaterialPropertiesName", "MaterialPropertiesName",
                            R"(This the name of the input material property field data array)"_help,
                            "material_properties"};
};

struct Wrapper5 {
    bool set(vtkDataSetRegionSurfaceFilter& filter) {
        filter.SetMaterialIDsName(property.get().c_str());
        return true;
    }
    StringProperty property{
        "MaterialIDsName", "MaterialIDsName",
        R"(This the name of the input and output material id field data array)"_help,
        "material_ids"};
};

struct Wrapper6 {
    bool set(vtkDataSetRegionSurfaceFilter& filter) {
        filter.SetMaterialPIDsName(property.get().c_str());
        return true;
    }
    StringProperty property{
        "MaterialPIDsName", "MaterialPIDsName",
        R"(This the name of the output material ancestry id field data array)"_help,
        "material_ancestors"};
};

struct Wrapper7 {
    bool set(vtkDataSetRegionSurfaceFilter& filter) {
        filter.SetInterfaceIDsName(property.get().c_str());
        return true;
    }
    StringProperty property{
        "InterfaceIDsName", "InterfaceIDsName",
        R"(This the name of the input and output interface id field data array)"_help,
        "interface_ids"};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkDataSetRegionSurfaceFilter> {
    static constexpr std::string_view className = "vtkDataSetRegionSurfaceFilter";
    static constexpr std::string_view identifier = "DataSetRegionSurfaceFilter";
    static constexpr std::string_view displayName = "Extract Region Surface";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataSet", -1, R"(This property specifies the input to the Extract Surface
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
    static constexpr std::string_view doc = R"(The Extract
Surface filter extracts the polygons forming the outer
surface of the input dataset. This filter operates on any
type of data and produces polygonal data as
output.)";
};

void registervtkDataSetRegionSurfaceFilter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkDataSetRegionSurfaceFilter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
