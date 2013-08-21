#include "Game.h"
#include "ui/TitleMenu.h"
#include "Debug.h"
#include "Model.h"
#include "Text.h"
#include "Player.h"
#include "IO.h"

Game::Game()
{
  Text::init();
  HUD::HUD::init();
}

Game&
Game::instance()
{
  static Game instance;
  return instance;
}

void
Game::start(const Json::Value& levelspec)
{
  DBG("Starting a new game");

  HUD::HUD::show();

  keyboard      = new KeyboardHandler();
  meta_manager  = new MetaManager();
  map           = new Map(levelspec);
  tower_manager = new TowerManager();
  creep_manager = new CreepManager(levelspec);
  player        = new Player();

  tower_manager->set_faction(Faction::Faction::SAGES);
  player->alter_gold(12000)->alter_lives(10);
}

void
Game::stop()
{
  delete player;
  delete creep_manager;
  delete tower_manager;
  delete map;
  delete meta_manager;
  delete keyboard;

  Text::clear();
  HUD::HUD::hide();

  ui::TitleMenu::instance().display();
  ui::TitleMenu::instance().show(200, ui::Menu::ANIM_LEFT);
}
