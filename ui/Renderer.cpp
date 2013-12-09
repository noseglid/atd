#include "ui/Renderer.h"
#include "engine/Video.h"
#include "ImageLoader.h"
#include "Debug.h"

#include <OpenGL/glu.h>

namespace ui {

Renderer::Renderer()
{
}

void
Renderer::RenderGeometry(
  Rocket::Core::Vertex* vertices,
  int num_vertices,
  int *indices,
  int num_indices,
  const Rocket::Core::TextureHandle texture,
  const Rocket::Core::Vector2f& translation)
{
  glPushMatrix();
  glTranslatef(translation.x, translation.y, 0);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  glVertexPointer(2, GL_FLOAT, sizeof(Rocket::Core::Vertex), &vertices[0].position);
  glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Rocket::Core::Vertex), &vertices[0].colour);

  if (!texture) {
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  } else {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, (GLuint) texture);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Rocket::Core::Vertex), &vertices[0].tex_coord);
  }

  glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, indices);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glPopMatrix();
}

Rocket::Core::CompiledGeometryHandle
Renderer::CompileGeometry(
  Rocket::Core::Vertex* vertices,
  int num_vertices,
  int *indices,
  int num_indices,
  const Rocket::Core::TextureHandle texture)
{
  return (Rocket::Core::CompiledGeometryHandle) NULL;
}

void
Renderer::RenderCompiledGeometry(
  Rocket::Core::CompiledGeometryHandle geometry,
  const Rocket::Core::Vector2f& translation)
{
}

void
Renderer::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry)
{
}

void
Renderer::EnableScissorRegion(bool enable)
{
  (enable ? glEnable : glDisable)(GL_SCISSOR_TEST);
}

void
Renderer::SetScissorRegion(int x, int y, int width, int height)
{
  engine::resolution res = engine::Video::instance().get_resolution();
  glScissor(x, res.height - (y + height), width, height);
}

bool
Renderer::LoadTexture(
  Rocket::Core::TextureHandle& texture_handle,
  Rocket::Core::Vector2i& texture_dimensions,
  const Rocket::Core::String& source)
{
  glm::ivec2 dimensions;
  if (0 == (texture_handle = IL::GL::texture(source.CString(), dimensions, false))) {
    return false;
  }

  texture_dimensions.x = dimensions.x;
  texture_dimensions.y = dimensions.y;

  return true;
}

bool
Renderer::GenerateTexture(
  Rocket::Core::TextureHandle& texture_handle,
  const Rocket::Core::byte* source,
  const Rocket::Core::Vector2i& dimensions)
{
  DBG("Generating texture with resolution " << dimensions.x << "," << dimensions.y << " from bytes.");

  GLuint texid = 0;
  glGenTextures(1, &texid);
  if (texid == 0) {
    DBGERR("Failed to generate textures.");
    return false;
  }

  glBindTexture(GL_TEXTURE_2D, texid);

  gluBuild2DMipmaps(
    GL_TEXTURE_2D,
    4,
    dimensions.x,
    dimensions.y,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    source
  );

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  texture_handle = texid;

  return true;
}

void
Renderer::ReleaseTexture(Rocket::Core::TextureHandle texture_handle)
{
  glDeleteTextures(1, (GLuint*) &texture_handle);
}

}
