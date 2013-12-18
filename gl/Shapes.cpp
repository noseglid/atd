#include "gl/Shapes.h"

#include <cmath>
#include <vector>
#include <OpenGL/gl.h>

namespace gl {

void
Shapes::circle(float radius, size_t points)
{
  glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
  glEnableClientState(GL_VERTEX_ARRAY);

  float step =  2 * M_PI / (float)points;

  std::vector<GLfloat> vertices;
  vertices.push_back(0.0f);
  vertices.push_back(0.0f);
  vertices.push_back(0.0f);
  for (int i = points; i >= 0; --i) {
    vertices.push_back(cos((float)i * step) * radius);
    vertices.push_back(0.0f);
    vertices.push_back(sin((float)i * step) * radius);
  }

  glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)&vertices[0]);
  glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size() / 3);

  glPopClientAttrib();
}

}
