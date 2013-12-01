#ifndef __GLTRANSFORM_H__
#define __GLTRANSFORM_H__

#include "gl/GL.h"
#include "gl/glm.h"

B_NS_GL

class Transform
{
public:
  static void billboard();
  static void enable2D(bool origo_top = false);
  static void disable2D();
  static glm::vec3 unproject(const int& x, const int& y);
};

E_NS_GL

#endif
