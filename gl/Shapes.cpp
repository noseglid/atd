#include "gl/Shapes.h"

#include <cmath>
#include <OpenGL/gl.h>

B_NS_GL

void
Shapes::circle(float radius, size_t points)
{
  float step =  2 * M_PI / (float)points;

  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(0.0f, 0.0f, 0.0f);
  for (int i = points; i >= 0; --i) {
    float x = cos((float)i * step) * radius;
    float z = sin((float)i * step) * radius;
    glVertex3f(x, 0.0f, z);
  }
  glEnd();
}

E_NS_GL
