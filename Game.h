#ifndef __GAME_H__
#define __GAME_H__

#include "Camera.h"
#include "Debug.h"
#include "Map.h"

#include <de.h>
#include <SDL/SDL.h>
#include <map>

struct GameEvent
{
	float elapsed;
	SDL_Event ev;

	GameEvent(float elapsed = 0.0f, SDL_Event ev = SDL_Event()) :
		elapsed(elapsed), ev(ev) {}
};

class Game : public de::Emitter<GameEvent>
{
	Camera *camera;
	Map *map;
	struct timeval start_time;
	float elapsed;
	bool running;

	Game();
	void operator=(Game const&);
	Game(const Game&);

	void handle_event(const SDL_Event& ev);

public:
	void init(Map *map);
	void run();
	void stop();

	static Game& instance();
};

#endif
