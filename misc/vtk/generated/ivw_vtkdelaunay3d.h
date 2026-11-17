#pragma once

#include <memory>

namespace inviwo {
class ProcessorFactoryObject;
namespace vtkwrapper {
std::unique_ptr<ProcessorFactoryObject> registervtkDelaunay3D();
}  // namespace vtkwrapper
}  // namespace inviwo
