#ifndef __GAME_H__
#define __GAME_H__

#include "Camera.h"
#include "Debug.h"
#include "Player.h"
#include "Map.h"
#include "MetaManager.h"
#include "KeyboardHandler.h"
#include "CreepManager.h"
#include "TowerManager.h"

#include <de.h>
#include <SDL/SDL.h>
#include <SDL/SDL_Mixer.h>
#include <map>

class Game : public de::Emitter<>
{
  bool running;
  HUD::InfoBox finishedbox;

  Game();
  void operator=(Game const&);
  Game(const Game&);

  void tick(const engine::Event& ev);

public:
  MetaManager *meta_manager;
  KeyboardHandler *keyboard;
  Player *player;
  Map *map;
  TowerManager *tower_manager;
  CreepManager *creep_manager;

  static Game& instance();

  void start(int levelid, const Json::Value& levelspec);
  void stop();
  bool is_running();
};

#endif
