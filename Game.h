#ifndef __GAME_H__
#define __GAME_H__

#include "Camera.h"
#include "Debug.h"
#include "Map.h"
#include "MetaManager.h"
#include "KeyboardHandler.h"
#include "CreepManager.h"
#include "TowerManager.h"

#include <de.h>
#include <SDL/SDL.h>
#include <SDL/SDL_Mixer.h>
#include <map>

class Game
{

  MetaManager *meta_manager;
  KeyboardHandler *keyboard;

  Game();
  void operator=(Game const&);
  Game(const Game&);

public:
  static Game& instance();

  void start();
};

#endif
