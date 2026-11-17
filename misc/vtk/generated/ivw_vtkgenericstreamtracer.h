#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkGenericStreamTracer();
}  // namespace vtkwrapper
}  // namespace inviwo
