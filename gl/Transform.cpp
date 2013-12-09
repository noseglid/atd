#include "gl/Transform.h"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

namespace gl {

void
Transform::billboard()
{
  glm::mat4 m(1.0);
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)&m);

  GLfloat *ptr = reinterpret_cast<GLfloat*>(&m);

  ptr[0] = 1.0f; ptr[1] = 0.0f; ptr[2]  = 0.0f;
  ptr[4] = 0.0f; ptr[5] = 1.0f; ptr[6]  = 0.0f;
  ptr[8] = 0.0f; ptr[9] = 0.0f; ptr[10] = 1.0f;

  glLoadMatrixf(&m[0][0]);
}

void
Transform::enable2D(bool origo_top)
{
  int vport[4];
  glGetIntegerv(GL_VIEWPORT, vport);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  if (origo_top) {
    glOrtho(0, vport[2], vport[3], 0, -1, 1);
  } else {
    glOrtho(0, vport[2], 0, vport[3], -1, 1);
  }

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  glLoadIdentity();
  glDisable(GL_LIGHTING);
}

void
Transform::disable2D()
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glEnable(GL_LIGHTING);
}

glm::vec3
Transform::unproject(const int& x, const int& y)
{
  GLint viewport[4];
  GLdouble modelview[16];
  GLdouble projection[16];
  GLfloat winX, winY, winZ;
  GLdouble posX, posY, posZ;

  glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  glGetIntegerv(GL_VIEWPORT, viewport);

  winX = (float)x;
  winY = (float)viewport[3] - (float)y;
  glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
  gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

  return glm::vec3(posX, posY, posZ);
}

}
