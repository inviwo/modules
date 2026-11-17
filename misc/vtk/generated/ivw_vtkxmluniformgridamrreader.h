#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkXMLUniformGridAMRReader();
}  // namespace vtkwrapper
}  // namespace inviwo
