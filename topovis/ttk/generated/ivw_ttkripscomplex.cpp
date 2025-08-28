#include "ivw_ttkripscomplex.h"
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
#include <ttkRipsComplex.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkRipsComplex& filter) {
        filter.SetSelectFieldsWithRegexp(property.get());
        return true;
    }
    BoolProperty property{"SelectFieldsWithRegexp", "Select Fields with a Regexp",
                          R"(Select input scalar fields matching a regular expression.)"_help,
                          false};
};

struct Wrapper1 {
    bool set(ttkRipsComplex& filter) {
        filter.SetRegexpString(property.get().c_str());
        return true;
    }
    StringProperty property{"Regexp", "Regexp",
                            R"(This regexp will be used to filter the chosen fields. Only
matching ones will be selected.)"_help,
                            ".*"};
};

struct Wrapper2 {
    bool set(ttkRipsComplex& filter) {
        filter.SetOutputDimension(property.get());
        return true;
    }
    IntProperty property{"OutputDimension",
                         "Output Dimension",
                         R"(Cell dimension of the generated Rips complex.)"_help,
                         2,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkRipsComplex& filter) {
        filter.SetEpsilon(property.get());
        return true;
    }
    DoubleProperty property{"Epsilon",
                            "Diameter (epsilon)",
                            R"(Distance threshold above with no cell should be generated.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkRipsComplex& filter) {
        filter.SetXColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"XColumn", "XColumn",
                            R"(This property specifies which data array is going to be
used as the X coordinate in the generated polydata
dataset.)"_help,
                            "Component_0"};
};

struct Wrapper5 {
    bool set(ttkRipsComplex& filter) {
        filter.SetYColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"YColumn", "YColumn",
                            R"(This property specifies which data array is going to be
used as the Y coordinate in the generated polydata
dataset.)"_help,
                            "Component_1"};
};

struct Wrapper6 {
    bool set(ttkRipsComplex& filter) {
        filter.SetZColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"ZColumn", "ZColumn",
                            R"(This property specifies which data array is going to be
used as the Z coordinate in the generated polydata
dataset.)"_help,
                            "Component_2"};
};

struct Wrapper7 {
    bool set(ttkRipsComplex& filter) {
        filter.SetKeepAllDataArrays(property.get());
        return true;
    }
    BoolProperty property{"KeepAllDataArrays", "Keep All Data Arrays",
                          R"(Keep all data arrays.)"_help, true};
};

struct Wrapper8 {
    bool set(ttkRipsComplex& filter) {
        filter.SetComputeGaussianDensity(property.get());
        return true;
    }
    BoolProperty property{"ComputeGaussianDensity", "Compute Gaussian Density",
                          R"(Should Gaussian density be computed on every vertex?)"_help, false};
};

struct Wrapper9 {
    bool set(ttkRipsComplex& filter) {
        filter.SetStdDev(property.get());
        return true;
    }
    DoubleProperty property{"StdDev",
                            "Standard Deviation",
                            R"(Gaussian density standard deviation)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(ttkRipsComplex& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper11 {
    bool set(ttkRipsComplex& filter) {
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

struct Wrapper12 {
    bool set(ttkRipsComplex& filter) {
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

struct Wrapper13 {
    bool set(ttkRipsComplex& filter) {
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

struct Wrapper14 {
    bool set(ttkRipsComplex& filter) {
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
struct VTKTraits<ttkRipsComplex> {
    static constexpr std::string_view uri = "ttk.ttkRipsComplex";
    static constexpr std::string_view className = "ttkRipsComplex";
    static constexpr std::string_view identifier = "ttkRipsComplex";
    static constexpr std::string_view displayName = "TTK RipsComplex";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {InputData{.identifier = "Input",
                                                                .dataType = "vtkTable",
                                                                .numComp = 1,
                                                                .doc = R"(Data-set to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options",
              {"SelectFieldsWithRegexp", "ScalarFields", "Regexp", "OutputDimension", "Epsilon",
               "XColumn", "YColumn", "ZColumn", "KeepAllDataArrays", "ComputeGaussianDensity",
               "StdDev"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc = R"ivw(TTK ripsComplex plugin documentation.

Online examples:

- https://topology-tool-kit.github.io/examples/persistentGenerators_householdAnalysis/

- https://topology-tool-kit.github.io/examples/persistentGenerators_periodicPicture/)ivw";
};

void registerttkRipsComplex(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkRipsComplex>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
