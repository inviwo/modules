#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkPOutlineCornerFilter();
}  // namespace vtkwrapper
}  // namespace inviwo
