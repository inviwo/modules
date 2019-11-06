#pragma once

#include <inviwo/core/common/inviwo.h>
#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <functional>

namespace inviwo {
/**
 * \class DeformableCube
 * \brief Data structure for a cube whose vertices can be manipulated by a supplied lambda
 * function
 */
class IVW_MODULE_TENSORVISBASE_API DeformableCube {
public:
    // DeformableCube() = delete;
    DeformableCube(const vec4& color = vec4(1.f));
    virtual ~DeformableCube() = default;

    void deform(const std::function<void(vec3& vertex)>& lambda, const bool& normalize = true);
    void deform(const std::function<void(vec3& vertex, vec4& color)>& lambda,
                const bool& normalize = true);
    void transform(const vec3& pos, const vec3& scale);
    std::shared_ptr<BasicMesh> getGeometry();

private:
    std::vector<size3_t> faces_;
    std::shared_ptr<BasicMesh> mesh_;

    void createCube(const vec4& color);
    void calculateNormals();
};

}  // namespace inviwo
