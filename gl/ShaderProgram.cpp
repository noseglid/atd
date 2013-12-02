#include "ShaderProgram.h"
#include "Exception.h"
#include "Debug.h"

B_NS_GL

ShaderProgram::ShaderProgram() : reference(glCreateProgram())
{
  DBG("Creating a new Shader program");
}

ShaderProgram::~ShaderProgram()
{
  glDeleteProgram(reference);
}

void
ShaderProgram::attachShader(Shader *shader)
{
  glAttachShader(reference, shader->reference);
}

void
ShaderProgram::link()
{
  glLinkProgram(reference);

  GLint program_ok;
  glGetProgramiv(reference, GL_LINK_STATUS, &program_ok);
  if (GL_TRUE != program_ok) {
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

GLint
ShaderProgram::getAttribLocation(std::string name)
{
  return glGetAttribLocation(reference, name.c_str());
}

GLint
ShaderProgram::getUniformLocation(std::string name)
{
  return glGetUniformLocation(reference, name.c_str());
}

ShaderProgram *
ShaderProgram::create(std::vector<std::string> vshaders, std::vector<std::string> fshaders)
{
  ShaderProgram *instance = new ShaderProgram();

  for (std::string vshader : vshaders) {
    Shader *shader = new Shader(GL_VERTEX_SHADER, std::string("resources/shaders/") + vshader);
    instance->attachShader(shader);
  }

  for (std::string fshader : fshaders) {
    Shader *shader = new Shader(GL_FRAGMENT_SHADER, std::string("resources/shaders/") + fshader);
    instance->attachShader(shader);
  }

  instance->link();

  return instance;
}

E_NS_GL
