#include "Game.h"
#include "Debug.h"
#include "Audio.h"
#include "Model.h"
#include "Text.h"
#include "Player.h"
#include "IO.h"

Game::Game()
{
}

Game&
Game::instance()
{
  static Game instance;
  return instance;
}

void
Game::start()
{
  DBG("Starting a new game");

  Text::init();
  HUD::HUD::init();

  Json::Value levelspec = Json::deserialize(IO::file_get_contents("resources/levels/test.map"));
  Map::instance().load(levelspec);

  TowerManager::instance().set_faction(Faction::Faction::SAGES);
  CreepManager::instance().setup(levelspec);

  keyboard      = new KeyboardHandler();
  meta_manager  = new MetaManager();

  Player::instance().alter_gold(12000);
  Player::instance().alter_lives(10);
}
