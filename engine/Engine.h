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

  Engine();

  void operator=(Engine const&);
  Engine(const Engine&);

  void handle_event(const SDL_Event& ev);

public:
  void run();
  void stop();
  float get_elapsed();

  static Engine& instance();
};

E_NS_ENGINE

#endif
