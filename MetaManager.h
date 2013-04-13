#ifndef __METAMANAGER_H__
#define __METAMANAGER_H__

#include <SDL/SDL.h>

struct GameEvent;

class MetaManager
{
	float current_fps;
	float last_measure;
	bool draw_meta;

public:
	MetaManager();
	void tick(const GameEvent& ev);
	void keydown(const GameEvent& ev);
};

#endif
