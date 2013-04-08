#ifndef __GAME_H__
#define __GAME_H__

#include "Camera.h"
#include "Debug.h"
#include "Map.h"

#include <de.h>
#include <SDL/SDL.h>
#include <map>

class Game : public de::Emitter<>
{
	Camera *camera;
	Map *map;
	struct timeval start_time;
	float elapsed;
	bool running;

	SDL_MouseMotionEvent mouse_motion_event;
	SDL_MouseButtonEvent mouse_button_event;
	SDL_KeyboardEvent keyboard_event;

	Game();
	void operator=(Game const&);
	Game(const Game&);

	void handle_event(const SDL_Event& ev);
	void handle_mouse_event(const SDL_Event& ev);

public:
	void init(Map *map);
	void run();
	void stop();

	SDL_MouseMotionEvent last_mouse_motion_event() const;
	SDL_MouseButtonEvent last_mouse_button_event() const;
	SDL_KeyboardEvent last_keyboard_event() const;
	float get_elapsed() const;

	static Game& instance();
};

#endif
