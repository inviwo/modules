#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkDistributedDataFilter();
}  // namespace vtkwrapper
}  // namespace inviwo
