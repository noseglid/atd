#include "gl/Material.h"

#include <OpenGL/gl.h>

B_NS_GL

Material::Material(
  glm::vec4 diffuse,
  glm::vec4 ambient,
  glm::vec4 specular,
  glm::vec4 emission,
  float shininess
) :
  diffuse(diffuse),
  ambient(ambient),
  specular(specular),
  emission(emission),
  shininess(shininess)
{
}

void
Material::use() const
{
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   &diffuse[0]);
  glMaterialfv(GL_FRONT, GL_AMBIENT,   &ambient[0]);
  glMaterialfv(GL_FRONT, GL_EMISSION,  &emission[0]);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  &specular[0]);
  glMaterialf (GL_FRONT, GL_SHININESS, shininess);
}

Material
Material::from_assimp(const aiMaterial *mtl)
{
  aiColor4D diffuse, ambient, specular, emissive;
  glm::vec4 rdiffuse, rambient, rspecular, remission;
  if (aiReturn_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
    rdiffuse = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);

  if (aiReturn_SUCCESS == mtl->Get(AI_MATKEY_COLOR_AMBIENT, ambient))
    rambient = glm::vec4(ambient.r, ambient.g, ambient.b, ambient.a);

  if (aiReturn_SUCCESS == mtl->Get(AI_MATKEY_COLOR_SPECULAR, specular))
    rspecular = glm::vec4(specular.r, specular.g, specular.b, specular.a);

  if (aiReturn_SUCCESS == mtl->Get(AI_MATKEY_COLOR_EMISSIVE, emissive))
    remission = glm::vec4(emissive.r, emissive.g, emissive.b, emissive.a);

  float shininess = 64.0f;
  mtl->Get(AI_MATKEY_SHININESS, shininess);

  return Material(rdiffuse, rambient, rspecular, remission, shininess);
}

void
Material::set_from_color(float r, float g, float b, float opacity)
{
  GLfloat
    diffuse_ambient[] = { r,    g,    b,    opacity },
    blank[]           = { 0.0f, 0.0f, 0.0f, 1.0f    };

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse_ambient);
  glMaterialfv(GL_FRONT, GL_SPECULAR, blank);
  glMaterialfv(GL_FRONT, GL_EMISSION, blank);
  glMaterialf (GL_FRONT, GL_SHININESS, 0.0f);
}

E_NS_GL
