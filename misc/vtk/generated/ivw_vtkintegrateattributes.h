#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkIntegrateAttributes();
}  // namespace vtkwrapper
}  // namespace inviwo
