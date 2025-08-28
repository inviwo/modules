#include "ivw_vtktabletopolydata.h"
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
#include <vtkTableToPolyData.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkTableToPolyData& filter) {
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
    bool set(vtkTableToPolyData& filter) {
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
    bool set(vtkTableToPolyData& filter) {
        filter.SetZColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"ZColumn", "ZColumn",
                            R"(This property specifies which data array is going to be
used as the Z coordinate in the generated polydata
dataset.)"_help,
                            ""};
};

struct Wrapper3 {
    bool set(vtkTableToPolyData& filter) {
        filter.SetCreate2DPoints(property.get());
        return true;
    }
    BoolProperty property{"Create2DPoints", "2D Points",
                          R"(Specify whether the points of the polydata are 3D or 2D.
If this is set to true then the Z Column will be ignored and the z
value of each point on the polydata will be set to 0. By default this
will be off.)"_help,
                          false};
};

struct Wrapper4 {
    bool set(vtkTableToPolyData& filter) {
        filter.SetPreserveCoordinateColumnsAsDataArrays(property.get());
        return true;
    }
    BoolProperty property{"KeepAllDataArrays", "KeepAllDataArrays",
                          R"(Allow user to keep columns specified as X,Y,Z as Data
arrays. By default this will be off.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkTableToPolyData> {
    static constexpr std::string_view uri = "vtk.vtkTableToPolyData";
    static constexpr std::string_view className = "vtkTableToPolyData";
    static constexpr std::string_view identifier = "TableToPolyData";
    static constexpr std::string_view displayName = "Table To Points";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkTable",
                  .numComp = 1,
                  .doc = R"(This property specifies the input..)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;

    static constexpr std::string_view doc = R"ivw(The
TableToPolyData filter converts a vtkTable to a set of
points in a vtkPolyData. One must specifies the columns in
the input table to use as the X, Y and Z coordinates for
the points in the output.)ivw";
};

void registervtkTableToPolyData(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkTableToPolyData>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
