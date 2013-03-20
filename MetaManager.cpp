#include "MetaManager.h"
#include "Text.h"
#include "Debug.h"
#include "Game.h"

#include <sstream>

#define FPS_MEASURE_INTERVAL 0.25f /* seconds */

MetaManager::MetaManager() : last_measure(0), draw_meta(false)
{
	Game& g = Game::instance();
	g.on("tick", std::bind(&MetaManager::tick, this));
	g.on("keydown", std::bind(&MetaManager::keydown, this));
}

void
MetaManager::tick()
{
	static int frames = 0;
	frames++;

	float elapsed = Game::instance().get_elapsed();

	if (elapsed - last_measure > FPS_MEASURE_INTERVAL) {
		current_fps = (float)(frames) / (elapsed - last_measure);
		frames = 0;
		last_measure = elapsed;
	}

	if (draw_meta) {
		std::stringstream ss;
		ss.precision(4);
		ss << "FPS: " << current_fps;
		Text::overlay(ss.str(), 10, 10);
	}
}

void
MetaManager::keydown()
{
	SDL_KeyboardEvent event = Game::instance().last_keyboard_event();
	switch (event.keysym.sym) {
		case SDLK_m: draw_meta = !draw_meta; break;
		default: break;
	}
}
