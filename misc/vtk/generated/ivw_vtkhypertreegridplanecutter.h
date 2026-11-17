#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkHyperTreeGridPlaneCutter();
}  // namespace vtkwrapper
}  // namespace inviwo
