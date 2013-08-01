#ifndef __KEYBOARDHANDLER_H__
#define __KEYBOARDHANDLER_H__

#include "engine/Engine.h"

class KeyboardHandler
{
public:
  KeyboardHandler();
  void keydown(const engine::Event& ev);
};

#endif
