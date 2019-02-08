/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <modules/openmesh/utils/meshproximity.h>

#pragma optimize("", off)

namespace inviwo {

// template <typename P, typename N>
TriMesh::Point distanceToTriangle(const TriMesh::Point p0, const TriMesh::Point p1,
                                  const TriMesh::Point p2, const TriMesh::Point p) {
    /* auto e0 = p1-p0;
     auto e1 = p2-p0;
     auto e2 = p2-p1;
     auto N = (e0 % e1).normalized();;
     auto d = N|(p-p0);
     auto proj =  p - N*d;

     auto a = e0|(proj-p0);
     auto b = e1|(proj-p0);
     auto c = e2|(proj-p0);


     if(b<=0){
         if(a<=0){
             std::cout << 1 << std::endl;
             return p0;
         }
         if(a>=1){
             std::cout << 2 << std::endl;
             return p1;
         }
         std::cout << 3 << std::endl;
         return p0 + a * e0;
     }
     if(a<=0){
         if(b>=1){
             std::cout << 4 << std::endl;
             return p2;
         }
         std::cout << 5 << std::endl;
         return p0 + b * e2;
     }
     if(c<=0 && a>=1){
         std::cout << 6 << std::endl;
         return p1;
     }if(c >= 1 && b>=1){
         std::cout << 7 << std::endl;
         return p2;
     }
     if(a+b>=1){
         std::cout << 8 << std::endl;
         return p1 + c * e2;
     }

     std::cout << 9 << std::endl;
     return proj;*/

    auto edge0 = p1 - p0;
    auto edge1 = p2 - p0;
    auto v0 = p0 - p;

    float a = edge0 | edge0;
    float b = edge0 | edge1;
    float c = edge1 | edge1;
    float d = edge0 | v0;
    float e = edge1 | v0;

    float det = a * c - b * b;
    float s = b * e - c * d;
    float t = b * d - a * e;

    if (s + t < det) {
        if (s < 0.f) {
            if (t < 0.f) {
                if (d < 0.f) {
                    s = glm::clamp(-d / a, 0.f, 1.f);
                    t = 0.f;
                } else {
                    s = 0.f;
                    t = glm::clamp(-e / c, 0.f, 1.f);
                }
            } else {
                s = 0.f;
                t = glm::clamp(-e / c, 0.f, 1.f);
            }
        } else if (t < 0.f) {
            s = glm::clamp(-d / a, 0.f, 1.f);
            t = 0.f;
        } else {
            float invDet = 1.f / det;
            s *= invDet;
            t *= invDet;
        }
    } else {
        if (s < 0.f) {
            float tmp0 = b + d;
            float tmp1 = c + e;
            if (tmp1 > tmp0) {
                float numer = tmp1 - tmp0;
                float denom = a - 2 * b + c;
                s = glm::clamp(numer / denom, 0.f, 1.f);
                t = 1 - s;
            } else {
                t = glm::clamp(-e / c, 0.f, 1.f);
                s = 0.f;
            }
        } else if (t < 0.f) {
            if (a + d > b + e) {
                float numer = c + e - b - d;
                float denom = a - 2 * b + c;
                s = glm::clamp(numer / denom, 0.f, 1.f);
                t = 1 - s;
            } else {
                s = glm::clamp(-e / c, 0.f, 1.f);
                t = 0.f;
            }
        } else {
            float numer = c + e - b - d;
            float denom = a - 2 * b + c;
            s = glm::clamp(numer / denom, 0.f, 1.f);
            t = 1.f - s;
        }
    }

    return p0 + s * edge0 + t * edge1;

    return {};
}

inviwo::vec3 openmeshutil::closestPoint(TriMesh &mesh, vec3 p_in) {
    if (!mesh.has_face_normals()) {
        mesh.request_face_normals();
        mesh.update_normals();
    }
    TriMesh::Point p{p_in.x, p_in.y, p_in.z};

    TriMesh::Point closest = p;
    float distance = std::numeric_limits<float>::max();
    size_t skipped = 0;
    for (auto &f_it : mesh.faces()) {
        if (mesh.valence(f_it) != 3) {
            skipped++;
            continue;
        }

        mesh.face(f_it);

        // const auto &faceNormal = mesh.face_normals_[f_it];

        auto v_it = mesh.cfv_begin(f_it);

        auto asdf = *v_it;
        auto i0 = *v_it++;
        auto i1 = *v_it++;
        auto i2 = *v_it;

        const auto &p0 = mesh.point(i0);
        const auto &p1 = mesh.point(i1);
        const auto &p2 = mesh.point(i2);

        /*  {
              auto d = (p - p0).norm();
              if (d < distance) {
                  distance = d;
                  closest = p0;
              }
          }
          {
              auto d = (p - p1).norm();
              if (d < distance) {
                  distance = d;
                  closest = p1;
              }
          }
          {
              auto d = (p - p2).norm();
              if (d < distance) {
                  distance = d;
                  closest = p2;
              }
          }*/
        auto res = distanceToTriangle(p0, p1, p2, p);
        auto dist = (res - p).norm();
        if (dist < distance) {
            distance = dist;
            closest = res;
        }
    }
    if (skipped) {
        LogWarnCustom("openmeshutil::toInviwo",
                      "Skipped " << skipped << " faces since they weren't triangles");
    }
    return {closest[0], closest[1], closest[2]};
}

}  // namespace inviwo
