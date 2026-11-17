#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkXMLPPolyDataReader();
}  // namespace vtkwrapper
}  // namespace inviwo
