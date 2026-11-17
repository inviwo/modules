#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkDecimatePolylineFilter();
}  // namespace vtkwrapper
}  // namespace inviwo
