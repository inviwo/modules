#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkEvenlySpacedStreamlines2D();
}  // namespace vtkwrapper
}  // namespace inviwo
