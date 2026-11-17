#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registerttkOFFReader();
}  // namespace vtkwrapper
}  // namespace inviwo
