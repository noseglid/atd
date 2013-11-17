#include "VBO.h"

VBO::VBO() : texture(0), count(0)
{

  glGenBuffers(BUFFER_TYPE_END, buffers);
}

VBO::~VBO()
{
  glDeleteBuffers(BUFFER_TYPE_END, buffers);
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
VBO::update_data(BUFFER_TYPE type, const std::vector<GLfloat>& data)
{
  GLsizei size = sizeof(data[0]) * data.size();

  glBindBuffer(GL_ARRAY_BUFFER, get_buffer(type));
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data.data());

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
VBO::set_texture(GLuint texture)
{
  this->texture = texture;
}

void
VBO::draw() const
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_TEXTURE_2D);

    glBindBuffer(GL_ARRAY_BUFFER, get_buffer(VERTEX));
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, get_buffer(NORMAL));
    glNormalPointer(GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, get_buffer(TEXCOORD));
    glTexCoordPointer(2, GL_FLOAT, 0, 0);

    glBindTexture(GL_TEXTURE_2D, 1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_buffer(INDICE));
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
