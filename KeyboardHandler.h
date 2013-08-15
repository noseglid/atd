#ifndef __KEYBOARDHANDLER_H__
#define __KEYBOARDHANDLER_H__

#include "engine/Engine.h"

class KeyboardHandler
{
  engine::Engine::id_t keydownev;

public:
  KeyboardHandler();
  ~KeyboardHandler();
  void keydown(const engine::Event& ev);
};

#endif
