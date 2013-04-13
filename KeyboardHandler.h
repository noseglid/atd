#ifndef __KEYBOARDHANDLER_H__
#define __KEYBOARDHANDLER_H__

struct GameEvent;

class KeyboardHandler
{
public:
	KeyboardHandler();
	void keydown(const GameEvent& ev);
};

#endif
