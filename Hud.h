#ifndef __HUD_H__
#define __HUD_H__

#include "TickListener.h"

class Hud : public TickListener
{
public:
	Hud();
	void tick();
};

#endif
