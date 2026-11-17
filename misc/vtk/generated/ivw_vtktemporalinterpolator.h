#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkTemporalInterpolator();
}  // namespace vtkwrapper
}  // namespace inviwo
