#include "KeyboardHandler.h"
#include "Game.h"

#include <OpenGL/gl.h>

KeyboardHandler::KeyboardHandler()
{
	Game::instance().on("keydown", std::bind(&KeyboardHandler::keydown, this, std::placeholders::_1));
}

void
KeyboardHandler::keydown(const GameEvent& ev)
{
	SDL_KeyboardEvent event = ev.ev.key;
	switch	(event.keysym.sym) {
		case SDLK_f:
			GLboolean fog;
			glGetBooleanv(GL_FOG, &fog);
			((GL_TRUE == fog) ? glDisable : glEnable)(GL_FOG);
			break;

		case SDLK_q:
			Game::instance().stop();
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
