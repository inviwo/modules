#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkStreamTracer();
}  // namespace vtkwrapper
}  // namespace inviwo
