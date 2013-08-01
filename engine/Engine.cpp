#include "Engine.h"
#include "Audio.h"

#include <sys/time.h>

B_NS_ENGINE

Engine::Engine() : running(false), elapsed(0.0f)
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

  emit(signal, Event(elapsed, ev));
}

void
Engine::run()
{
  SDL_Event ev;
  struct timeval start_time, now;
  running = true;

  gettimeofday(&start_time, NULL);

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

    gettimeofday(&now, NULL);
    elapsed = now.tv_sec - start_time.tv_sec;
    if (now.tv_usec < start_time.tv_usec) {
      elapsed += (now.tv_usec - start_time.tv_usec) / 1000000.0f;
    } else {
      elapsed -= (start_time.tv_usec - now.tv_usec) / 1000000.0f;
    }

    glEnable(GL_DEPTH_TEST);
    emit("tick", engine::Event(elapsed));

    glDisable(GL_DEPTH_TEST);
    emit("tick_nodepth", engine::Event(elapsed));

    SDL_GL_SwapBuffers();
  }
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
