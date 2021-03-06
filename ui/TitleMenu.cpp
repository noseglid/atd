#include "ui/TitleMenu.h"
#include "ui/LevelSelectMenu.h"
#include "ui/SetUserMenu.h"
#include "ui/OptionsMenu.h"
#include "Shutdown.h"
#include "Debug.h"
#include "User.h"

#include <pjson.hpp>

namespace ui {

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

  auto changedcb = [this]() {
    document->GetElementById("placeholder-hero")
            ->SetInnerRML(User::instance().get_username().c_str());
  };
  changedcb();

  User::instance().on("changed", changedcb);
}

void
TitleMenu::display(bool visible)
{
  static Audio& audio = Audio::instance();
  static Mix_Music *mus = audio.load_music("menu.ogg");

  if (visible && !audio.music_is_playing()) {
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

}
