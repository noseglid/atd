#include "ui/TitleMenu.h"
#include "Exception.h"
#include "Debug.h"
#include "Text.h"
#include "hud/HUD.h"
#include "engine/Video.h"
#include "TowerManager.h"
#include "Faction.h"
#include "CreepManager.h"
#include "IO.h"
#include "Player.h"
#include "Game.h"

#include <pjson.hpp>

B_NS_UI

class NewGameListener  : public Rocket::Core::EventListener
{
  bool processed;
  public:
  NewGameListener() : processed(false) {}

  void ProcessEvent(Rocket::Core::Event& event)
  {
    if (processed) return;
    processed = true;
    TitleMenu::instance().hide();
    Game::instance().start();

  }
} new_game_listener;

class ExitListener : public Rocket::Core::EventListener
{
  void ProcessEvent(Rocket::Core::Event& event)
  {
    throw Exception("User clicked exit in title menu");
  }
} exit_listener;

TitleMenu::TitleMenu()
{
  document = UI::instance().load("resources/rml/title.rml");
  document->GetElementById("newgame")->AddEventListener("click", &new_game_listener);
  document->GetElementById("exit")->AddEventListener("click", &exit_listener);
  document->Show();
}

void
TitleMenu::hide()
{
  document->Hide();
}

TitleMenu&
TitleMenu::instance()
{
  static TitleMenu instance;
  return instance;
}

E_NS_UI
