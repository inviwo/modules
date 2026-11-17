#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkExtractCTHPart();
}  // namespace vtkwrapper
}  // namespace inviwo
