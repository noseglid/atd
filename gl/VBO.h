#ifndef __VBO_H__
#define __VBO_H__

#include "Debug.h"
#include "gl/GL.h"
#include "gl/Material.h"

#include <OpenGL/gl.h>

#include <vector>
#include <map>

B_NS_GL

class VBO
{
public:
  enum BUFFER_TYPE {
    VERTEX,
    NORMAL,
    TEXCOORD,
    INDICE,
    BUFFER_TYPE_END
  };

private:
  Material mtl;
  GLuint texture;
  GLsizei count;
  GLuint buffers[BUFFER_TYPE_END];

  struct attrib {
    GLint location;
    GLuint buffer;
    GLint size;
    GLenum type;
    GLsizei stride;
    char offset;
  };

  std::vector<attrib> attribs;

  VBO(const VBO& rhs) = delete;
  VBO& operator=(const VBO& rhs) = delete;

public:
  VBO();
  ~VBO();

  GLuint get_buffer(BUFFER_TYPE type) const;

  void bind_data(BUFFER_TYPE type, const std::vector<GLfloat>& data, GLenum usage = GL_STATIC_DRAW);
  void bind_indices(const std::vector<GLushort>& data);

  template <class T>
  GLuint create_vertex_attrib(const std::vector<T>& data)
  {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data[0]) * data.size(), &data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return buffer;
  }
  void set_vertex_attrib(
    GLint location,
    GLuint buffer,
    GLint size,
    GLenum type,
    GLsizei stride,
    char offset
  );

  void set_texture(GLuint texture);
  void set_material(const Material& mtl);

  void draw() const;
};

E_NS_GL

#endif
