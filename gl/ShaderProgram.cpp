#include "ShaderProgram.h"
#include "Exception.h"
#include "Debug.h"

B_NS_GL

ShaderProgram::ShaderProgram() : reference(glCreateProgram())
{
}

ShaderProgram::~ShaderProgram()
{
  for (Shader *s : shaders) {
    glDeleteShader(s->reference);
    delete s;
  }

  glDeleteProgram(reference);
}

void
ShaderProgram::attachShader(Shader *shader)
{
  shaders.push_back(shader);
  glAttachShader(reference, shader->reference);
}

void
ShaderProgram::link()
{
  glLinkProgram(reference);

  GLint program_ok;
  glGetProgramiv(reference, GL_LINK_STATUS, &program_ok);
  if (!program_ok) {
    DBGERR("Failed to link shader program");

    GLint log_length;
    glGetProgramiv(reference, GL_INFO_LOG_LENGTH, &log_length);

    char *log = (char*)malloc(log_length);
    glGetProgramInfoLog(reference, log_length, NULL, log);
    DBG(std::endl << std::endl << log);
    free(log);

    throw Exception("Could not link ShaderProgram.");
  }
}

void
ShaderProgram::use()
{
  glUseProgram(reference);
}

void
ShaderProgram::disuse()
{
  glUseProgram(0);
}

E_NS_GL
