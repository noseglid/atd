#ifndef __GL__SHADERPROGRAM_H__
#define __GL__SHADERPROGRAM_H__

#include "gl/GL.h"
#include "gl/Shader.h"

#include <stack>
#include <vector>
#include <map>

B_NS_GL

class ShaderProgram
{
public:
  enum predef {
    PROGRAM_DEFAULT,
    PROGRAM_TEXTURE,
    PROGRAM_COLOR,
    PROGRAM_SKYBOX
  };

private:
  GLuint reference;
  GLint idle_program;

  std::vector<Shader*> shaders;

  ShaderProgram(const ShaderProgram& rhs);
  void operator=(const ShaderProgram& rhs);

  ShaderProgram();

  static std::stack<predef> program_stack;
  static std::map<predef, ShaderProgram *> cache;

public:

  ~ShaderProgram();

  /**
   * Attaching a shader yields ownership to ShaderProgram.
   */
  void attachShader(Shader *shader);

  void link();

  void use();
  void reset();

  GLint getAttribLocation(std::string name);
  GLint getUniformLocation(std::string name);

  static ShaderProgram* create(std::vector<std::string> vshaders, std::vector<std::string> fshaders);
  static void push(predef program);
  static void pop();
};

E_NS_GL

#endif
