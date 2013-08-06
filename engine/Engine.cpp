#include "Engine.h"
#include "Audio.h"

#include <sys/time.h>

B_NS_ENGINE

Engine::Engine() : running(false)
{
}

void
Engine::handle_event(const SDL_Event& ev)
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
Engine::run()
{
  SDL_Event ev;
  running = true;

  /***********************************
   * Should not responsible for this */
  Camera::instance().set_limits(
    1.0f, 16 - 1.0f,
    4.0f, 12.0f,
    1.0f, 16 - 1.0f
  );
  Mix_Music *bgmusic = Audio::instance().load_music("music/meadow.ogg");
  Audio::instance().play(bgmusic);
  /***********************************/

  while (running) {
    while (SDL_PollEvent(&ev)) {
      handle_event(ev);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Camera::instance().orientate();

    GLfloat pos[] = { 0.5, 0.5, 0.5, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    glPushMatrix();
    glEnable(GL_DEPTH_TEST);
    emit("tick", engine::Event(get_elapsed()));
    glPopMatrix();

    glPushMatrix();
    glDisable(GL_DEPTH_TEST);
    emit("tick_nodepth", engine::Event(get_elapsed()));
    glPopMatrix();


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

Engine&
Engine::instance()
{
  static Engine instance;
  return instance;
}

E_NS_ENGINE
