#include "MetaManager.h"
#include "text/Dispatcher.h"
#include "Debug.h"
#include "engine/Engine.h"
#include "utils/Color.h"
#include "gl/Transform.h"

#include <sstream>

#define FPS_MEASURE_INTERVAL 1.00f /* seconds */

MetaManager::MetaManager() : last_measure(0), draw_meta(false)
{
  DBG("Registering events for MetaManager");
  engine::Engine& e = engine::Engine::instance();
  events.push_back(
    e.on("tick", std::bind(&MetaManager::tick, this, std::placeholders::_1))
  );
  events.push_back(
    e.on("keydown", std::bind(&MetaManager::keydown, this, std::placeholders::_1))
  );
}

MetaManager::~MetaManager()
{
  DBG("Deregistering MetaManager from events");
  using engine::Engine;
  for (Engine::id_t ev : events) {
    Engine::instance().off(ev);
  }
}

void
MetaManager::tick(const engine::Event& ev)
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
    gl::Transform::enable2D();
    text::Dispatcher::overlay(ss.str(), 20, 100, utils::colors::gray, 16.0f, false, true);
    gl::Transform::disable2D();
  }
}

void
MetaManager::keydown(const engine::Event& ev)
{
  SDL_KeyboardEvent event = ev.ev.key;
  switch (event.keysym.sym) {
    case SDLK_m: draw_meta = !draw_meta; break;
    default: break;
  }
}
