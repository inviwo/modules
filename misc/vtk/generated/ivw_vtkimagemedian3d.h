#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkImageMedian3D();
}  // namespace vtkwrapper
}  // namespace inviwo
