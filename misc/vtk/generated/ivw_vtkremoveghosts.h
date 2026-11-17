#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkRemoveGhosts();
}  // namespace vtkwrapper
}  // namespace inviwo
