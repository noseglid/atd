#include "VBO.h"

namespace gl {

VBO::VBO() : texture(0), count(0)
{
  glGenBuffers(BUFFER_TYPE_END, buffers);
}

VBO::~VBO()
{
  glDeleteBuffers(BUFFER_TYPE_END, buffers);

  for (auto attrib : attribs) {
    glDeleteBuffers(1, &attrib.buffer);
  }
}

GLuint
VBO::get_buffer(BUFFER_TYPE type) const
{
  return buffers[type];
}

void
VBO::bind_data(BUFFER_TYPE type, const std::vector<GLfloat>& data, GLenum usage)
{
  glBindBuffer(GL_ARRAY_BUFFER, get_buffer(type));
  glBufferData(GL_ARRAY_BUFFER, sizeof(data[0]) * data.size(), &data[0], usage);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
VBO::bind_indices(const std::vector<GLushort>& data)
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_buffer(INDICE));
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(data[0]) * data.size(),
               data.data(),
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  this->count = data.size();
}

void
VBO::set_vertex_attrib(
  GLint location,
  GLuint buffer,
  GLint size,
  GLenum type,
  GLsizei stride,
  char offset
)
{
  if (-1 == location) {
    DBGERR("Cannot use attrib location: " << location);
    return;
  }

  attrib a;
  a.location = location;
  a.buffer   = buffer;
  a.size     = size;
  a.type     = type;
  a.stride   = stride;
  a.offset   = offset;

  attribs.push_back(a);
}

void
VBO::set_texture(GLuint texture)
{
  this->texture = texture;
}

void
VBO::set_material(const Material& mtl)
{
  this->mtl = mtl;
}

void
VBO::draw() const
{
  glBindTexture(GL_TEXTURE_2D, this->texture);
  this->mtl.use();

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, get_buffer(VERTEX));
  glVertexPointer(3, GL_FLOAT, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, get_buffer(NORMAL));
  glNormalPointer(GL_FLOAT, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, get_buffer(TEXCOORD));
  glTexCoordPointer(2, GL_FLOAT, 0, 0);

  for (auto attrib : attribs) {
    glEnableVertexAttribArray(attrib.location);
    glBindBuffer(GL_ARRAY_BUFFER, attrib.buffer);
    glVertexAttribPointer(
      attrib.location,
      attrib.size,
      attrib.type,
      GL_FALSE,
      attrib.stride,
      (const GLvoid*)attrib.offset
    );
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_buffer(INDICE));
  glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);

  for (auto attrib : attribs) {
    glDisableVertexAttribArray(attrib.location);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindTexture(GL_TEXTURE_2D, 0);
}

}
