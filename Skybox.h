#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "gl/VBO.h"

#include <OpenGL/gl.h>

class Skybox
{
  gl::VBO vbo;

public:
  Skybox();

  void draw() const;
};

#endif
