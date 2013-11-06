#include "Game.h"
#include "text/Dispatcher.h"
#include "ui/TitleMenu.h"
#include "ui/PauseMenu.h"
#include "Debug.h"
#include "dal/Dal.h"

Game::Game() :
  running(false),
  finishedbox(text::Stream(), HUD::InfoBox::SNAP_CENTER),
  ev_done(engine::Engine::instance().invalid_eventid())
{
  HUD::HUD::init();

  engine::Engine::instance().on(
    "tick_nodepth", std::bind(&Game::tick, this, std::placeholders::_1)
  );
}

Game&
Game::instance()
{
  static Game instance;
  return instance;
}

void
Game::tick(const engine::Event& ev)
{
  finishedbox.draw();
}

void
Game::start(int levelid, const Json::Value& levelspec)
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

  auto finishedcb = [this](utils::Color c, std::string msg) {
    text::Stream ts = text::Stream();
    ts << text::Stream::size(32.0f) << c << msg;
    finishedbox.set_content(ts);
    ev_done = engine::Engine::instance().queue_event(7.0f, std::bind(&Game::stop, this, false));
  };

  creep_manager->on("complete", [finishedcb, levelid, this]() {
    finishedcb(utils::colors::green, "Level complete");
    dal::get()->set_level_completed(levelid, [levelid, this](bool success) {
      if (!success) {
        DBGERR("Could not set level complete for levelid: " << levelid);
        return;
      }
    });
  });

  player->on(
    "death", std::bind(finishedcb, utils::colors::red, "Crater lost")
  );

  running = true;
  emit("start");
}

void
Game::stop(bool clear_ev_done)
{
  delete player;
  delete creep_manager;
  delete tower_manager;
  delete map;
  delete meta_manager;
  delete keyboard;

  text::Dispatcher::instance().clear();
  HUD::HUD::hide();
  finishedbox.clear();

  ui::PauseMenu::instance()._hide();
  ui::TitleMenu::instance().display();
  ui::TitleMenu::instance().show(200, ui::Menu::ANIM_LEFT);

  if (clear_ev_done && ev_done != engine::Engine::instance().invalid_eventid()) {
    engine::Engine::instance().remove_event(ev_done);
  }

  ev_done = engine::Engine::instance().invalid_eventid();

  running = false;
  emit("stop");
}

bool
Game::is_running()
{
  return running;
}

void
Game::escape()
{
  if (tower_manager->escape()) {
    return;
  }

  ui::PauseMenu::instance().toggle();
}
