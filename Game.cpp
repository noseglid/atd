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
  int screen_width = 1024, screen_height = 768;
  Text::init(screen_width, screen_height);
  HUD::HUD::init(screen_width, screen_height);

  Json::Value levelspec = Json::deserialize(IO::file_get_contents("levels/test.map"));
  Map::instance().load(levelspec);

  TowerManager::instance().set_faction(Faction::Faction::SAGES);
  CreepManager::instance().setup(levelspec);

  keyboard      = new KeyboardHandler();
  meta_manager  = new MetaManager();

  Player::instance().alter_gold(12000);
  Player::instance().alter_lives(10);
}
