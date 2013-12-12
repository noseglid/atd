#ifndef __MAP__HEIGHTMAP_H__
#define __MAP__HEIGHTMAP_H__

#include <SDL_Image.h>
#include <string>

namespace map {

class Heightmap {
  SDL_Surface *data;
  float maxheight;

public:
  Heightmap(std::string, float maxheight);

  float sample(float x, float y) const;
};

}

#endif
