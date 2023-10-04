#include "ivw_vtkrectilineargridtopointset.h"

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
#include <vtkRectilinearGridToPointSet.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkRectilinearGridToPointSet> {
    static constexpr std::string_view className = "vtkRectilinearGridToPointSet";
    static constexpr std::string_view identifier = "RectilinearGridToPointSet";
    static constexpr std::string_view displayName = "Rectilinear Data to Point Set";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkRectilinearGrid", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;
    static constexpr std::string_view doc = R"(The Rectilinear Grid to Point Set
filter takes an rectilinear grid object and outputs an
equivalent Structured Grid (which is a type of point set). This
brings the data to a broader category of data storage but only
adds a small amount of overhead. This filter can be helpful in
applying filters that expect or manipulate point
coordinates.)";
};

void registervtkRectilinearGridToPointSet(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkRectilinearGridToPointSet>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
