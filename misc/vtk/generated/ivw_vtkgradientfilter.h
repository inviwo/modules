#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkGradientFilter();
}  // namespace vtkwrapper
}  // namespace inviwo
