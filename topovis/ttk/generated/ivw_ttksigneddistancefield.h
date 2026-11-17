#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registerttkSignedDistanceField();
}  // namespace vtkwrapper
}  // namespace inviwo
