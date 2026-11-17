#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkHyperTreeGridContour();
}  // namespace vtkwrapper
}  // namespace inviwo
