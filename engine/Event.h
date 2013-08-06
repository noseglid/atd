#ifndef __ENGINE__EVENT_H__
#define __ENGINE__EVENT_H__

#include <SDL/SDL.h>

namespace engine {

struct Event
{
  float elapsed;
  SDL_Event ev;

  Event(float elapsed = 0.0f, SDL_Event ev = SDL_Event()) :
    elapsed(elapsed), ev(ev) {}
};

}

#endif
