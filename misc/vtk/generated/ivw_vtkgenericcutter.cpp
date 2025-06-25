#include "ivw_vtkgenericcutter.h"
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
#include <vtkGenericCutter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkGenericCutter> {
    static constexpr std::string_view uri = "vtk.vtkGenericCutter";
    static constexpr std::string_view className = "vtkGenericCutter";
    static constexpr std::string_view identifier = "GenericCut";
    static constexpr std::string_view displayName = "Slice Generic Dataset";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkGenericDataSet", -1, R"(Set the input to the Generic Cut filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;

    static constexpr std::string_view doc = R"ivw(The
Generic Cut filter extracts the portion of the input data
set that lies along the specified plane or sphere. From
the Cut Function menu, you can select whether cutting will
be performed with a plane or a sphere. The appropriate 3D
widget (plane widget or sphere widget) will be displayed.
The parameters of the cut function can be specified
interactively using the 3D widget or manually using the
traditional user interface controls. Instructions for
using these 3D widgets and their corresponding user
interfaces are found in section 7.4. By default, the cut
lies on the specified plane or sphere. Using the Cut
Offset Values portion of the interface, it is also
possible to cut the data set at some offset from the
original cut function. The Cut Offset Values are in the
spatial units of the data set. To add a single offset,
select the value from the New Value slider in the Add
value portion of the interface and click the Add button,
or press Enter. To instead add several evenly spaced
offsets, use the controls in the Generate range of values
section. Select the number of offsets to generate using
the Number of Values slider. The Range slider controls the
interval in which to generate the offsets. Once the number
of values and range have been selected, click the Generate
button. The new offsets will be added to the Offset Values
list. To delete a value from the Cut Offset Values list,
select the value and click the Delete button. (If no value
is selected, the last value in the list will be removed.)
Clicking the Delete All button removes all the values in
the list. The Generic Cut filter takes a generic dataset
as input. Use the Input menu to choose a data set to cut.
The output of this filter is polygonal
data.)ivw";
};

void registervtkGenericCutter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkGenericCutter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
