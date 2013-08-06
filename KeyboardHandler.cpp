#include "KeyboardHandler.h"
#include "engine/Engine.h"

#include <OpenGL/gl.h>

KeyboardHandler::KeyboardHandler()
{
  engine::Engine::instance().on("keydown", std::bind(&KeyboardHandler::keydown, this, std::placeholders::_1));
}

void
KeyboardHandler::keydown(const engine::Event& ev)
{
  SDL_KeyboardEvent event = ev.ev.key;
  switch  (event.keysym.sym) {
    case SDLK_q:
      engine::Engine::instance().stop();
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
