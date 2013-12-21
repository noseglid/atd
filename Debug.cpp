#if DEBUG == 1

#include "Debug.h"

std::ostream& operator<<(std::ostream& out, const glm::vec2& vec)
{
  return out << "(" << vec.x << "," << vec.y << ")";
}

std::ostream& operator<<(std::ostream& out, const glm::vec3& vec)
{
  return out << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
}

#endif
