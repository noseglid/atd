#include "ui/TitleMenu.h"
#include "ui/LevelSelectMenu.h"
#include "ui/SetUserMenu.h"
#include "ui/OptionsMenu.h"
#include "Shutdown.h"
#include "Debug.h"

#include <pjson.hpp>

B_NS_UI

class TitleListener : public Rocket::Core::EventListener
{
  void ProcessEvent(Rocket::Core::Event& event)
  {
    Menu *next = NULL;
    auto el = event.GetCurrentElement();

    if (el->GetId() == "newgame") {
      next = &LevelSelectMenu::instance();
    }
    if (el->GetId() == "setuser") {
      next = &SetUserMenu::instance();
    }
    if (el->GetId() == "options") {
      next = &OptionsMenu::instance();
    }
    if (el->GetId() == "exit") {
      TitleMenu::instance().hide(200, []() {
        throw Shutdown("Clicked 'exit' in title menu.");
      }, Menu::ANIM_RIGHT);
      return;
    }

    TitleMenu::instance().hide(200, [next]() {
      next->show(200, []() {}, Menu::ANIM_LEFT);
    }, Menu::ANIM_LEFT);
  }
} title_listener;


TitleMenu::TitleMenu() : Menu("resources/rml/title.rml")
{
  document->GetElementById("newgame")->AddEventListener("click", &title_listener);
  document->GetElementById("setuser")->AddEventListener("click", &title_listener);
  document->GetElementById("options")->AddEventListener("click", &title_listener);
  document->GetElementById("exit")   ->AddEventListener("click", &title_listener);
  display(true);
}

void
TitleMenu::display(bool visible)
{
  if (visible) {
    Mix_Music *mus = Audio::instance().load_music("menu.ogg");
    Audio::instance().play(mus);
  }

  Menu::display(visible);
}

TitleMenu&
TitleMenu::instance()
{
  static TitleMenu instance;
  return instance;
}

E_NS_UI
