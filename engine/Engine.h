#ifndef __ENGINE__ENGINE_H__
#define __ENGINE__ENGINE_H__

#define B_NS_ENGINE namespace engine {
#define E_NS_ENGINE }

#include "engine/Event.h"
#include "Camera.h"

#include <de.h>

B_NS_ENGINE

class Engine : public de::Emitter<engine::Event>
{
  bool running;
  float elapsed;
  std::map<float, std::function<void()>> queued_events;

  Engine();

  void operator=(Engine const&);
  Engine(const Engine&);

  void handle_sdl_event(const SDL_Event& ev);
  void handle_queued_events();

public:
  void run();
  void stop();
  float get_elapsed();

  void queue_event(float seconds, std::function<void()> cb);

  static Engine& instance();
};

E_NS_ENGINE

#endif