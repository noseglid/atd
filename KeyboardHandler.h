#ifndef __KEYBOARDHANDLER_H__
#define __KEYBOARDHANDLER_H__

#include "Listener.h"

class KeyboardHandler : public Listener
{
public:
	KeyboardHandler();
	void event(const SDL_KeyboardEvent& event);
};

#endif
