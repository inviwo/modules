#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkAppendArcLength();
}  // namespace vtkwrapper
}  // namespace inviwo
