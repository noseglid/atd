#ifndef __GL__SHADERPROGRAM_H__
#define __GL__SHADERPROGRAM_H__

#include "gl/GL.h"
#include "gl/Shader.h"

#include <vector>

B_NS_GL

class ShaderProgram
{
  GLuint reference;

  std::vector<Shader*> shaders;

public:
  ShaderProgram();
  ~ShaderProgram();

  /**
   * Attaching a shader yields ownership to ShaderProgram.
   */
  void attachShader(Shader *shader);

  void link();

  void use();
  void disuse();

  GLint getAttribLocation(std::string name);
  GLint getUniformLocation(std::string name);
};

E_NS_GL

#endif
