#include "ImageLoader.h"
#include "Debug.h"
#include "Exception.h"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <SDL_image.h>
#include <iostream>
#include <fstream>
#include <algorithm>

BEGIN_NAMESPACE_IL

static std::map<std::string, GLuint> loaded_textures;

GLuint
GL::texture(std::string path, Vector2& dimensions, bool set_searchdir)
{
  if (set_searchdir) {
    path = "textures/" + path;
  }

  auto it = loaded_textures.find(path);
  if (it != loaded_textures.end()) {
    return it->second;
  }

  DBG("Loading texture from: " << path);

  GLuint texid;
  glGenTextures(1, &texid);

  glBindTexture(GL_TEXTURE_2D, texid);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  SDL_Surface *image = IMG_Load(path.c_str());
  if (NULL == image) {
    DBGERR("Could not load texture from '" << path << "', error: " << IMG_GetError());
    return 0;
  }

  GLenum format;
  switch (image->format->BytesPerPixel) {
  case 3:
    format = (image->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR;
    break;
  case 4:
    format = (image->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;
    break;
  default:
    DBGERR("Unknown image BPP (probably not true color).");
    SDL_FreeSurface(image);
    return 0;
  }

  gluBuild2DMipmaps(
    GL_TEXTURE_2D,
    image->format->BytesPerPixel,
    image->w,
    image->h,
    format,
    GL_UNSIGNED_BYTE,
    image->pixels
  );

  dimensions.x = image->w;
  dimensions.y = image->h;
  loaded_textures.insert(std::make_pair(path, texid));
  return texid;
}

GLuint
GL::texture(std::string path, bool set_searchdir)
{
  Vector2 v;
  return GL::texture(path, v, set_searchdir);
}

END_NAMESPACE_IL
