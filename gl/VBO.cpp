#include "VBO.h"

B_NS_GL

VBO::VBO() : shininess(0.0), texture(0), count(0)
{
  ambient[0]  = ambient[1]  = ambient[2]  = 0.2; ambient[3]  = 1.0;
  diffuse[0]  = diffuse[1]  = diffuse[2]  = 0.8; diffuse[3]  = 1.0;
  specular[0] = specular[1] = specular[2] = 0.0; specular[3] = 1.0;
  emission[0] = emission[1] = emission[2] = 0.0; emission[3] = 1.0;

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
  glMaterialfv(GL_FRONT, GL_AMBIENT,   ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuse);
  glMaterialfv(GL_FRONT, GL_EMISSION,  emission);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  specular);
  glMaterialf (GL_FRONT, GL_SHININESS, shininess);
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

E_NS_GL
