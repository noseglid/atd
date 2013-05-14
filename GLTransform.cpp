#include "GLTransform.h"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

void
GLTransform::billboard()
{
  Matrix4 m;
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)&m);

  m.c1r1 = m.c2r2 = m.c3r3 = 1.0f;
  m.c1r2 = m.c1r3 = 0.0f;
  m.c2r1 = m.c2r3 = 0.0f;
  m.c3r1 = m.c3r2 = 0.0f;

  glLoadMatrixf((GLfloat*)&m);
}

void
GLTransform::enable2D()
{
  int vport[4];
  glGetIntegerv(GL_VIEWPORT, vport);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  glOrtho(0, vport[2], 0, vport[3], -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  glLoadIdentity();
  glDisable(GL_LIGHTING);
}

void
GLTransform::disable2D()
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glEnable(GL_LIGHTING);
}

Vector3
GLTransform::unproject(const int& x, const int& y)
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

  return Vector3(posX, posY, posZ);
}
