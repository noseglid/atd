#ifndef __METAMANAGER_H__
#define __METAMANAGER_H__

#include "Listener.h"

#include <SDL/SDL.h>

class MetaManager : public Listener
{
	float current_fps;
	float last_measure;
	bool draw_meta;

public:
	MetaManager();
	void event(const float& elapsed);
	void event(const SDL_KeyboardEvent& event);
};

#endif
