#include "VBO.h"

VBO::VBO() : texture(0), count(0)
{
  ambient[0] = diffuse[0] = emission[0] = specular[0] = -1.0;
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
VBO::mtl_ambient(GLfloat ambient[4])
{
  memcpy(this->ambient, ambient, 4 * sizeof(GLfloat));
}

void
VBO::mtl_diffuse(GLfloat diffuse[4])
{
  memcpy(this->diffuse, diffuse, 4 * sizeof(GLfloat));
}

void
VBO::mtl_emission(GLfloat emission[4])
{
  memcpy(this->emission, emission, 4 * sizeof(GLfloat));
}

void
VBO::mtl_specular(GLfloat specular[4], GLfloat shininess)
{
  memcpy(this->specular, specular, 4 * sizeof(GLfloat));
  this->shininess = shininess;
}

void
VBO::use_material() const
{
  if (ambient[0] > 0.0) {
    glMaterialfv(GL_FRONT, GL_AMBIENT,   ambient);
  }

  if (diffuse[0] > 0.0) {
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuse);
  }

  if (emission[0] > 0.0) {
    glMaterialfv(GL_FRONT, GL_EMISSION,  emission);
  }

  if (specular[0] > 0.0) {
    glMaterialfv(GL_FRONT, GL_SPECULAR,  specular);
    glMaterialf (GL_FRONT, GL_SHININESS, shininess);
  }
}

void
VBO::draw() const
{
  if (0 != this->texture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->texture);
  }

  this->use_material();

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, get_buffer(VERTEX));
  glVertexPointer(3, GL_FLOAT, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, get_buffer(NORMAL));
  glNormalPointer(GL_FLOAT, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, get_buffer(TEXCOORD));
  glTexCoordPointer(2, GL_FLOAT, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_buffer(INDICE));
  glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindTexture(GL_TEXTURE_2D, 0);
}
