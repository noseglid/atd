#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include <OpenGL/gl.h>

class Skybox
{
  GLuint back, front, left, right, top, bottom;

public:
  Skybox();

  void draw() const;
};

#endif
