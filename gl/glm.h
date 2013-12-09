#ifndef __GL__GLM_H__
#define __GL__GLM_H__

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#pragma clang diagnostic pop

namespace gl {

class glmvec3cmp
{
public:
  bool operator()(const glm::vec3& lhs, const glm::vec3& rhs) const
  {
    if (lhs.z < rhs.z) return true;
    if (lhs.z == rhs.z && lhs.y < rhs.y) return true;
    if (lhs.z == rhs.z && lhs.y == rhs.y && lhs.x < rhs.x) return true;
    return false;
  }
};

}

#endif
