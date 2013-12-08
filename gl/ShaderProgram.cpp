#include "ShaderProgram.h"
#include "Exception.h"
#include "Debug.h"

B_NS_GL

std::stack<ShaderProgram::predef> ShaderProgram::program_stack;
std::map<ShaderProgram::predef, ShaderProgram *> ShaderProgram::cache;

ShaderProgram::ShaderProgram() : reference(glCreateProgram()), idle_program(0)
{
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
  glGetIntegerv(GL_CURRENT_PROGRAM, &idle_program);
  glUseProgram(reference);
}

void
ShaderProgram::reset()
{
  glUseProgram(idle_program);
  idle_program = 0;
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

void
ShaderProgram::push(predef program)
{
  auto it = cache.find(program);

  if (cache.end() == it) {
    ShaderProgram *shader;
    switch (program) {
    case PROGRAM_DEFAULT:
      shader = gl::ShaderProgram::create({ "default.v.glsl" }, { "default.f.glsl", "fog.f.glsl" });
      break;

    case PROGRAM_TEXTURE:
      shader = gl::ShaderProgram::create({ "default.v.glsl" }, { "texture.f.glsl", "fog.f.glsl" });
      break;

    case PROGRAM_COLOR:
      shader = gl::ShaderProgram::create({ "default.v.glsl" }, { "color.f.glsl", "fog.f.glsl" });
      break;

    case PROGRAM_SKYBOX:
      shader = gl::ShaderProgram::create({ "default.v.glsl" }, { "skybox.f.glsl", "fog.f.glsl" });
      break;

    }

    auto res = cache.insert(std::make_pair(program, shader));
    it = res.first;
  }

  program_stack.push(program);
  it->second->use();
}

void
ShaderProgram::pop()
{
  predef program = program_stack.top();
  program_stack.pop();
  auto it = cache.find(program);
  if (cache.end() == it) {
    DBGERR("Program not found after pop.");
    return;
  }

  it->second->reset();
}

E_NS_GL
