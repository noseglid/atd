#include "MetaManager.h"
#include "Text.h"
#include "Debug.h"
#include "Game.h"

#include <sstream>

#define FPS_MEASURE_INTERVAL 2.00f /* seconds */

MetaManager::MetaManager() : last_measure(0), draw_meta(false)
{
  Game& g = Game::instance();
  g.on("tick", std::bind(&MetaManager::tick, this, std::placeholders::_1));
  g.on("keydown", std::bind(&MetaManager::keydown, this, std::placeholders::_1));
}

void
MetaManager::tick(const GameEvent& ev)
{
  static int frames = 0;
  frames++;

  if (ev.elapsed - last_measure > FPS_MEASURE_INTERVAL) {
    current_fps = (float)(frames) / (ev.elapsed - last_measure);
    frames = 0;
    last_measure = ev.elapsed;
  }

  if (draw_meta) {
    std::stringstream ss;
    ss.precision(4);
    ss << "FPS: " << current_fps;
    Text::overlay(ss.str(), 100, 20, false, false);
  }
}

void
MetaManager::keydown(const GameEvent& ev)
{
  SDL_KeyboardEvent event = ev.ev.key;
  switch (event.keysym.sym) {
    case SDLK_m: draw_meta = !draw_meta; break;
    default: break;
  }
}
