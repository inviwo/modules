#include "ivw_ttkripscomplex.h"

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
#include "ttkRipsComplex.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkRipsComplex& filter) {
        filter.SetSelectFieldsWithRegexp(property.get());
        return true;
    }
    BoolProperty property{"SelectFieldsWithRegexp", "Select Fields with a Regexp", false};
};

struct Wrapper1 {
    bool set(ttkRipsComplex& filter) {
        filter.SetRegexpString(property.get().c_str());
        return true;
    }
    StringProperty property{"Regexp", "Regexp", ".*"};
};

struct Wrapper2 {
    bool set(ttkRipsComplex& filter) {
        filter.SetOutputDimension(property.get());
        return true;
    }
    IntProperty property{"OutputDimension", "Output Dimension", 2,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkRipsComplex& filter) {
        filter.SetEpsilon(property.get());
        return true;
    }
    DoubleProperty property{"Epsilon", "Diameter (epsilon)", 1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkRipsComplex& filter) {
        filter.SetXColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"XColumn", "XColumn", "Component_0"};
};

struct Wrapper5 {
    bool set(ttkRipsComplex& filter) {
        filter.SetYColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"YColumn", "YColumn", "Component_1"};
};

struct Wrapper6 {
    bool set(ttkRipsComplex& filter) {
        filter.SetZColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"ZColumn", "ZColumn", "Component_2"};
};

struct Wrapper7 {
    bool set(ttkRipsComplex& filter) {
        filter.SetKeepAllDataArrays(property.get());
        return true;
    }
    BoolProperty property{"KeepAllDataArrays", "Keep All Data Arrays", true};
};

struct Wrapper8 {
    bool set(ttkRipsComplex& filter) {
        filter.SetComputeGaussianDensity(property.get());
        return true;
    }
    BoolProperty property{"ComputeGaussianDensity", "Compute Gaussian Density", false};
};

struct Wrapper9 {
    bool set(ttkRipsComplex& filter) {
        filter.SetStdDev(property.get());
        return true;
    }
    DoubleProperty property{"StdDev", "Standard Deviation", 1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(ttkRipsComplex& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper11 {
    bool set(ttkRipsComplex& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper12 {
    bool set(ttkRipsComplex& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper13 {
    bool set(ttkRipsComplex& filter) {
        filter.SetCompactTriangulationCacheSize(property.get());
        return true;
    }
    DoubleProperty property{"CompactTriangulationCacheSize", "Cache", 0.2,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkRipsComplex> {
    static constexpr std::string_view identifier = "ttkRipsComplex";
    static constexpr std::string_view displayName = "TTK RipsComplex";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkTable", 1}};
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
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13>
        properties;
};

void registerttkRipsComplex(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkRipsComplex>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
