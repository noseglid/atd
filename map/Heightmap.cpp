#include "map/Heightmap.h"
#include "Exception.h"
#include "Debug.h"
#include "gl/glm.h"

namespace map {

Heightmap::Heightmap(std::string image, float maxheight) : maxheight(maxheight)
{
  data = IMG_Load(image.c_str());
  if (NULL == data) {
    throw Exception("Could not load image from: " + image);
  }

  DBG("Heightmap (" << data->w << "x" << data->h << "@"
    << (int)data->format->BitsPerPixel << "bpp" << ") loaded.");

  if (32 != data->format->BitsPerPixel) {
    throw Exception("Only 32bpp images supported for heightmap.");
  }
}

float
Heightmap::sample(float x, float y) const
{
  int pixel_w = glm::clamp(x, 0.0f, 1.0f) * (data->w - 1);
  int pixel_h = glm::clamp(y, 0.0f, 1.0f) * (data->h - 1);
  uint32_t *pixels = static_cast<uint32_t*>(data->pixels);

  struct pixel_t {
    uint8_t r, g, b, a;
  };

  pixel_t *pixel = (pixel_t*)&pixels[(pixel_h * data->w) + pixel_w];
  return maxheight * glm::clamp(((int)pixel->r) / 255.0f, 0.0f, 1.0f);
}

}
