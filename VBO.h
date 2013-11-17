#ifndef __VBO_H__
#define __VBO_H__

#include "Debug.h"

#include <OpenGL/gl.h>

#include <vector>

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
  GLuint texture;
  GLsizei count;
  GLuint buffers[4];

  VBO(const VBO& rhs) = delete;
  VBO& operator=(const VBO& rhs) = delete;

public:
  VBO();
  ~VBO();

  GLuint get_buffer(BUFFER_TYPE type) const;

  void bind_data(BUFFER_TYPE type, const std::vector<GLfloat>& data, GLenum usage = GL_STATIC_DRAW);
  void bind_indices(const std::vector<GLushort>& data);
  void update_data(BUFFER_TYPE type, const std::vector<GLfloat>& data);

  void set_texture(GLuint texture);

  void draw() const;
};

#endif
