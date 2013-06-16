#include "Game.h"
#include "Audio.h"
#include "Debug.h"
#include "Model.h"
#include "Text.h"

#include <sys/time.h>

Game::Game() : running(true)
{
}

void
Game::handle_event(const SDL_Event& ev)
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

  emit(signal, GameEvent(elapsed, ev));
}

void
Game::run()
{
  SDL_Event ev;
  struct timeval start_time, now;

  Map& map = Map::instance();
  Camera::instance().set_limits(
    1.0f, (float)map.get_width() - 1.0f,
    4.0f, 12.0f,
    1.0f, (float)map.get_height() - 1.0f
  );

  gettimeofday(&start_time, NULL);

  Mix_Music *bgmusic = Audio::instance().load_music("music/meadow.ogg");
  Audio::instance().play(bgmusic);

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
    emit("tick", GameEvent(elapsed));

    glDisable(GL_DEPTH_TEST);
    emit("tick_nodepth", GameEvent(elapsed));

    SDL_GL_SwapBuffers();
  }
}

void
Game::stop()
{
  running = false;
}

Game&
Game::instance()
{
  static Game instance;
  return instance;
}
