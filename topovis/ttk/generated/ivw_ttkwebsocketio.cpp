#include "ivw_ttkwebsocketio.h"
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
#include <ttkWebSocketIO.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkWebSocketIO& filter) {
        filter.SetPortNumber(property.get());
        return true;
    }
    IntProperty property{"PortNumber",
                         "PortNumber",
                         R"(The port number of WebSocket server.)"_help,
                         9285,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{64000, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkWebSocketIO& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper2 {
    bool set(ttkWebSocketIO& filter) {
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

struct Wrapper3 {
    bool set(ttkWebSocketIO& filter) {
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

struct Wrapper4 {
    bool set(ttkWebSocketIO& filter) {
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

struct Wrapper5 {
    bool set(ttkWebSocketIO& filter) {
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
struct VTKTraits<ttkWebSocketIO> {
    static constexpr std::string_view uri = "ttk.ttkWebSocketIO";
    static constexpr std::string_view className = "ttkWebSocketIO";
    static constexpr std::string_view identifier = "WebSocketIO";
    static constexpr std::string_view displayName = "TTK WebSocketIO";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1,
                  R"(The input vtkDataObject that will be send to all connected clients.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"(This module provides functions for bi-directional communication between two websockets, where the heavy lifting is done by the websocketpp library. The primary use case for this module is to send data that was computed in a c++ environment to a browser client, which can then freely process/render the data via JavaScript and HTML. To this end, this module needs to run a websocket server, to which a client can connect to via the ttkWebSocketIO.js library. The server can also receive data from the client, which is then fed into the processEvent function. In order to add custom processing of events, one needs to inherit from the WebSocketIO class and override the processEvent method. The vtk wrapper ttkWebSocketIO provides an example on how this abstract base module can be specialized to the application of sending/receiving vtkDataObjects. Specifically, the ttkWebSocketIO filter will send its input vtkDataObject as a serialized JSON object to all connected clients every time the filter is called with a new input. When the server receives a serialized JSON object form the client, then the filter will instantiate a vtkDataObject and pass it as the filter output.)";
};

void registerttkWebSocketIO(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkWebSocketIO>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
