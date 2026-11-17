#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registerttkMatrixToHeatMap();
}  // namespace vtkwrapper
}  // namespace inviwo
