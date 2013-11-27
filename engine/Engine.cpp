#include "Engine.h"
#include "Audio.h"
#include "Debug.h"

#include <sys/time.h>

B_NS_ENGINE

Engine::Engine() : running(false)
{
}

void
Engine::handle_sdl_event(const SDL_Event& ev)
{
  std::string signal;
  switch (ev.type) {
    case SDL_QUIT:            stop();                 return;
    case SDL_KEYDOWN:         signal = "keydown";     break;
    case SDL_KEYUP:           signal = "keyup";       break;
    case SDL_MOUSEBUTTONDOWN: signal = "mousedown";   break;
    case SDL_MOUSEBUTTONUP:   signal = "mouseup";     break;
    case SDL_MOUSEMOTION:     signal = "mousemotion"; break;
  }

  emit(signal, Event(get_elapsed(), ev));
}

void
Engine::handle_queued_events()
{
  auto it = queued_events.begin();
  while (it != queued_events.end()) {
    if (it->first > get_elapsed()) break;

    (it->second)();
    queued_events.erase(it++);
  }
}

void
Engine::run()
{
  SDL_Event ev;
  running = true;

  while (running) {
    while (SDL_PollEvent(&ev)) {
      handle_sdl_event(ev);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    Camera::instance().orientate();

    GLfloat pos[] = { 0.5, 0.7, 0.5, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    glPushMatrix();
    glEnable(GL_DEPTH_TEST);
    emit("tick", engine::Event(get_elapsed()));
    glPopMatrix();

    glPushMatrix();
    glDisable(GL_DEPTH_TEST);
    emit("tick_nodepth", engine::Event(get_elapsed()));
    glPopMatrix();

    handle_queued_events();

    SDL_GL_SwapBuffers();
  }
}

float
Engine::get_elapsed()
{
  return SDL_GetTicks() / 1000.0f;
}

void
Engine::stop()
{
  running = false;
}

Engine::eventid_t
Engine::invalid_eventid() const
{
  return queued_events.end();
}

Engine::eventid_t
Engine::queue_event(float seconds, std::function<void()> cb)
{
  return queued_events.insert(std::make_pair(get_elapsed() + seconds, cb)).first;
}

void
Engine::remove_event(eventid_t id)
{
  queued_events.erase(id);
}

Engine&
Engine::instance()
{
  static Engine instance;
  return instance;
}

E_NS_ENGINE
