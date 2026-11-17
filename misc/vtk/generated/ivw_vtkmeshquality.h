#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkMeshQuality();
}  // namespace vtkwrapper
}  // namespace inviwo
