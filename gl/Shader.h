#ifndef __GL__SHADER_H__
#define __GL__SHADER_H__

#include <OpenGL/gl.h>
#include <string>

namespace gl {

class Shader
{
  friend class ShaderProgram;

  GLuint reference;

  Shader(GLenum type, std::string file);
};

}

#endif
