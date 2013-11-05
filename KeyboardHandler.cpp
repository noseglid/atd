#include "KeyboardHandler.h"
#include "Debug.h"
#include "Game.h"
#include "engine/Engine.h"
#include "ui/PauseMenu.h"

#include <OpenGL/gl.h>

KeyboardHandler::KeyboardHandler()
{
  DBG("Registering events for KeyboardHandler");
  keydownev = engine::Engine::instance().on(
    "keydown", std::bind(&KeyboardHandler::keydown, this, std::placeholders::_1)
  );
}

KeyboardHandler::~KeyboardHandler()
{
  DBG("Deregistering KeyboardHandler from events");
  engine::Engine::instance().off(keydownev);
}

void
KeyboardHandler::keydown(const engine::Event& ev)
{
  SDL_KeyboardEvent event = ev.ev.key;
  switch  (event.keysym.sym) {
    case SDLK_q:
      Game::instance().stop();
      break;

    case SDLK_p:
    case SDLK_ESCAPE:
      Game::instance().escape();
      break;

    case SDLK_w:
      GLint polygon_mode[2];
      glGetIntegerv(GL_POLYGON_MODE, polygon_mode);
      polygon_mode[0] = (polygon_mode[0] == GL_FILL) ? GL_LINE : GL_FILL;
      polygon_mode[1] = (polygon_mode[1] == GL_FILL) ? GL_LINE : GL_FILL;
      glPolygonMode(GL_FRONT, polygon_mode[0]);
      glPolygonMode(GL_BACK, polygon_mode[1]);
      break;

    default:
      break;
  }
}
