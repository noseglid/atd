#ifndef __LISTENER_H__
#define __LISTENER_H__

#include "Debug.h"

#include <SDL/SDL.h>
#include <vector>
#include <typeinfo>

/* Pray that these numbers never collide with SDL_* defines */
enum ListenerEvents
{
	LE_TICK = 1000
};

enum ListenerPriority
{
	LP_LOW,
	LP_MEDIUM,
	LP_HIGH
};

class Listener
{

public:
	virtual ~Listener() {};

	virtual void event(const SDL_KeyboardEvent& event)
	{
		DBGWRN("Default implementation of keyboard event");
	}

	virtual void event(const SDL_MouseMotionEvent& event)
	{
		DBGWRN("Default implementation of mouse motion event");
	}

	virtual void event(const SDL_MouseButtonEvent& event)
	{
		DBGWRN("Default implementation of mouse button event");
	}

	/**
	 * Tick event, called ~60 times per second
	 */
	virtual void event(const float& elapsed)
	{
		DBGWRN("Default implementation of tick event");
	}
};

struct ListenerEntry {
	Listener *listener;
	std::vector<unsigned short> events;
	ListenerPriority priority;

	ListenerEntry& operator<<(unsigned int event) {
		events.push_back(event);
		return *this;
	}

	ListenerEntry& operator<<(enum ListenerEvents event)
	{
		events.push_back(event);
		return *this;
	}
};

#endif
