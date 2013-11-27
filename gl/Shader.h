#ifndef __GL__SHADER_H__
#define __GL__SHADER_H__

#include "gl/GL.h"

#include <OpenGL/gl.h>
#include <string>

B_NS_GL

class Shader
{
  friend class ShaderProgram;

  GLuint reference;

public:
  Shader(GLenum type, std::string file);
};

E_NS_GL

#endif
