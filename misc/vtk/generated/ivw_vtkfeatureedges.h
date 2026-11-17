#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkFeatureEdges();
}  // namespace vtkwrapper
}  // namespace inviwo
