#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registerttkCinemaWriter();
}  // namespace vtkwrapper
}  // namespace inviwo
