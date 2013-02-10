#include "MetaManager.h"
#include "Text.h"
#include "Debug.h"
#include "Game.h"

#include <sstream>

#define FPS_MEASURE_INTERVAL 0.25f /* seconds */

MetaManager::MetaManager() : last_measure(0), draw_meta(false)
{
	Game::instance().register_listener(this) << SDL_KEYDOWN << LE_TICK;
}

void
MetaManager::event(const float& elapsed)
{
	static int frames = 0;
	frames++;

	if (elapsed - last_measure > FPS_MEASURE_INTERVAL) {
		current_fps = (float)(frames) / (elapsed - last_measure);
		frames = 0;
		last_measure = elapsed;
	}

	if (draw_meta) {
		std::stringstream ss;
		ss.precision(4);
		ss << "FPS: " << current_fps;
		Text::render(ss.str(), 10, 10);
	}
}

void
MetaManager::event(const SDL_KeyboardEvent& event)
{
	switch (event.keysym.sym) {
		case SDLK_m: draw_meta = !draw_meta; break;
		default: break;
	}
}
