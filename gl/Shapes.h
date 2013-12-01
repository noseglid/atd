#ifndef __GL__SHAPES_H__
#define __GL__SHAPES_H__

#include "gl/GL.h"

#include <string>

B_NS_GL

class Shapes
{
public:
  static void circle(float radius, size_t points = 20);
};

E_NS_GL

#endif
