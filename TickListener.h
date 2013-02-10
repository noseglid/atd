#ifndef __TICKLISTENER_H__
#define __TICKLISTENER_H__

#include "Listener.h"

class TickListener : public Listener
{
public:
	virtual ~TickListener() {};
	virtual void tick() = 0;
};

#endif
