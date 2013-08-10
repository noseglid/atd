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
Game::start(const std::string& levelname)
{
  DBG("Starting a new game");

  Mix_Music *bgmusic = Audio::instance().load_music("meadow.ogg");
  Audio::instance().play(bgmusic);

  Text::init();
  HUD::HUD::init();
  HUD::HUD::show();

  std::string levelfile = "resources/levels/" + levelname + ".map";
  Json::Value levelspec = Json::deserialize(IO::file_get_contents(levelfile));
  Map::instance().load(levelspec);

  TowerManager::instance().set_faction(Faction::Faction::SAGES);
  CreepManager::instance().setup(levelspec);

  keyboard      = new KeyboardHandler();
  meta_manager  = new MetaManager();

  Player::instance().alter_gold(12000);
  Player::instance().alter_lives(10);
}
