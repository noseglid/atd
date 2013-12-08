#ifndef __ENGINE__ENGINE_H__
#define __ENGINE__ENGINE_H__

#define B_NS_ENGINE namespace engine {
#define E_NS_ENGINE }

#include "engine/Event.h"
#include "Camera.h"
#include "Skybox.h"

#include <de.h>

B_NS_ENGINE

class Engine : public de::Emitter<engine::Event>
{
  bool running;
  float elapsed;
  Skybox *skybox;

  typedef std::map<float, std::function<void()>> events_t;
  events_t queued_events;

  Engine();

  void operator=(Engine const&);
  Engine(const Engine&);

  void handle_sdl_event(const SDL_Event& ev);
  void handle_queued_events();

public:
  typedef events_t::const_iterator eventid_t;

  void run();
  void stop();
  float get_elapsed();

  eventid_t invalid_eventid() const;
  eventid_t queue_event(float seconds, std::function<void()> cb);
  void remove_event(eventid_t id);

  static Engine& instance();
};

E_NS_ENGINE

#endif
