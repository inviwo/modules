#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkGenericContourFilter();
}  // namespace vtkwrapper
}  // namespace inviwo
