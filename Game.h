#ifndef __GAME_H__
#define __GAME_H__

#include "Camera.h"
#include "Listener.h"
#include "Debug.h"

#include <SDL/SDL.h>
#include <map>

class Game
{
	Camera *camera;
	struct timeval start_time;
	float elapsed;
	bool running;

	std::multimap<ListenerPriority, ListenerEntry *> listeners;

	Game();
	void operator=(Game const&);
	Game(const Game&);

	void run_ticks();
	void handle_event(const SDL_Event& ev);
	void handle_mouse_event(const SDL_Event& ev);

public:
	void init();
	void run();
	void stop();

	static Game& instance();

	ListenerEntry& register_listener(Listener *listener, enum ListenerPriority priority = LP_LOW);
};

#endif
