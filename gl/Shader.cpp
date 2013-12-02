#include "gl/Shader.h"
#include "Exception.h"
#include "Debug.h"
#include "IO.h"

B_NS_GL

Shader::Shader(GLenum type, std::string file)
{
  DBG("Creating a new shader (" << ((type == GL_FRAGMENT_SHADER) ? "fragment" : "vertex") << ")"
    << " from file: " << file);
  std::string src = IO::file_get_contents(file);
  const GLchar *c_str_src = src.c_str();
  const GLint src_length  = src.size();

  reference = glCreateShader(type);
  glShaderSource(reference, 1, &c_str_src, &src_length);
  glCompileShader(reference);

  GLint shader_ok;
  glGetShaderiv(reference, GL_COMPILE_STATUS, &shader_ok);
  if (!shader_ok) {
    DBGERR("Could not compile shader:");

    GLint log_length;
    glGetShaderiv(reference, GL_INFO_LOG_LENGTH, &log_length);

    char *log = (char*)malloc(log_length);
    glGetShaderInfoLog(reference, log_length, NULL, log);
    DBG(std::endl << std::endl << log);
    free(log);

    throw Exception("Could not compile shader.");
  }
}

E_NS_GL
