#include "Skybox.h"
#include "ImageLoader.h"
#include "Camera.h"
#include "gl/glm.h"
#include "Debug.h"

#include <vector>
#include <array>
#include <map>

#define SBW 2400.0 // SkyboxWidth
#define SBH 1800.0 // SkyboxHeight

static std::map<std::string, std::array<float, 4>> texcoords = {
  { "left", { {
    (0.0000 * SBW + 0.5) / SBW,
    (0.3333 * SBH + 0.5) / SBH,
    (0.2500 * SBW - 0.5) / SBW,
    (0.6667 * SBH - 0.5) / SBH,
  } } },
  { "front",  { {
    (0.2500 * SBW + 0.5) / SBW,
    (0.3333 * SBH + 0.5) / SBH,
    (0.5000 * SBW - 0.5) / SBW,
    (0.6667 * SBH - 0.5) / SBH,
  } } },
  { "right",  { {
    (0.5000 * SBW + 0.5) / SBW,
    (0.3333 * SBH + 0.5) / SBH,
    (0.7500 * SBW - 0.5) / SBW,
    (0.6667 * SBH - 0.5) / SBH,
  } } },
  { "back",   { {
    (0.7500 * SBW + 0.5) / SBW,
    (0.3333 * SBH + 0.5) / SBH,
    (1.0000 * SBW - 0.5) / SBW,
    (0.6667 * SBH - 0.5) / SBH,
  } } },
  { "top",    { {
    (0.2500 * SBW + 0.5) / SBW,
    (0.0000 * SBH + 0.5) / SBH,
    (0.5000 * SBW - 0.5) / SBW,
    (0.3333 * SBH - 0.5) / SBH,
  } } },
  { "bottom", { {
    (0.2500 * SBW + 0.5) / SBW,
    (0.6667 * SBH + 0.5) / SBH,
    (0.5000 * SBW - 0.5) / SBW,
    (1.0000 * SBH - 0.5) / SBH
  } } },
};

Skybox::Skybox()
{
  GLuint texture = IL::GL::texture("skybox.png", true, GL_NEAREST_MIPMAP_LINEAR);

  /* Rendered in this order:
   * 0 +-------+ 3
   *   |\      |
   *   |  \    |
   *   |    \  |
   *   |      \|
   * 1 +-------+ 2
   *
   * Triangle 1: 0 -> 1 -> 2
   * Triangle 2: 2 -> 3 -> 0
   *
   * For each side of box
   */

  std::vector<GLfloat> vertices {
    /* Left quad */
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    /* Front quad */
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,

    /* Right quad */
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    /* Back quad */
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    /* Top quad */
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,

    /* Bottom quad */
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f
  };

  /* Why does this needs to be stored in a variable before accessing?? */
  std::array<float, 4>
    left   = texcoords["left"],
    front  = texcoords["front"],
    right  = texcoords["right"],
    back   = texcoords["back"],
    top    = texcoords["top"],
    bottom = texcoords["bottom"];

  std::vector<GLfloat> texcoords {
    /* Left quad */
    left[2], left[1],
    left[2], left[3],
    left[0], left[3],
    left[0], left[1],

    /* Front quad */
    front[0], front[1],
    front[0], front[3],
    front[2], front[3],
    front[2], front[1],

    /* Right quad */
    right[0], right[1],
    right[0], right[3],
    right[2], right[3],
    right[2], right[1],

    /* Back quad */
    back[2], back[1],
    back[2], back[3],
    back[0], back[3],
    back[0], back[1],

    /* Top quad */
    top[0], top[1],
    top[0], top[3],
    top[2], top[3],
    top[2], top[1],

    /* Bottom quad */
    bottom[0], bottom[3],
    bottom[0], bottom[1],
    bottom[2], bottom[1],
    bottom[2], bottom[3],
  };

  std::vector<GLushort> indices {
    /* Left quad */
    0, 3, 2, 2, 1, 0,

    /* Front quad */
    4, 5, 6, 6, 7, 4,

    /* Right quad */
    8, 9, 10, 10, 11, 8,

    /* Back quad */
    12, 15, 14, 14, 13, 12,

    /* Top quad */
    16, 17, 18, 18, 19, 16,

    /* Bottom quad */
    20, 23, 22, 22, 21, 20
  };

  vbo.set_texture(texture);
  vbo.bind_data(gl::VBO::VERTEX, vertices);
  vbo.bind_data(gl::VBO::TEXCOORD, texcoords),
  vbo.bind_indices(indices);
}

void
Skybox::draw() const
{
  glPushMatrix();
  glLoadIdentity();
  Camera& c = Camera::instance();
  glm::mat4 matrix = glm::lookAt(glm::vec3(0.0f), c.get_direction(), glm::vec3(0.0, 1.0, 0.0));
  glMultMatrixf(&matrix[0][0]);

  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);

  vbo.draw();

  glPopAttrib();
  glPopMatrix();
}
