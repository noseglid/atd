#ifndef __GL__MATERIAL_H__
#define __GL__MATERIAL_H__

#include "gl/glm.h"

#include <assimp/material.h>

namespace gl {

class Material
{
public:
  enum TYPE {
    DIFFUSE,
    AMBIENT,
    SPECULAR,
    EMISSION
  };

private:
  glm::vec4 diffuse, ambient, specular, emission;
  float shininess;

public:

  Material(
    glm::vec4 diffuse  = glm::vec4(0.8, 0.8, 0.8, 1.0),
    glm::vec4 ambient  = glm::vec4(0.2, 0.2, 0.2, 1.0),
    glm::vec4 specular = glm::vec4(0.0, 0.0, 0.0, 1.0),
    glm::vec4 emission = glm::vec4(0.0, 0.0, 0.0, 1.0),
    float shininess    = 0.0
  );

  void use() const;

  static Material from_assimp(const aiMaterial *mtl);
  static void set_from_color(float r, float g, float b, float opacity = 1.0f);
};

}

#endif
