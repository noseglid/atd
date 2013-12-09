#include "ui/PauseMenu.h"
#include "ui/TitleMenu.h"
#include "ui/OptionsMenu.h"
#include "Game.h"
#include "Debug.h"

namespace ui {

class PauseMenuListener : public Rocket::Core::EventListener
{
  void ProcessEvent(Rocket::Core::Event& event)
  {
    Menu *next = NULL;
    auto el = event.GetCurrentElement();

    if (el->GetId() == "options") {
      next = &OptionsMenu::instance();
    }

    if (el->GetId() == "exit") {
      PauseMenu::instance().hide(200, []() {
        PauseMenu::instance().toggle();
        Game::instance().stop();
      }, Menu::ANIM_RIGHT);
      return;
    }

    if (NULL == next) {
      DBGWRN("No next menu set. Ignore.");
      return;
    }

    PauseMenu::instance().hide(200, [next]() {
      next->show(200, []() {}, Menu::ANIM_LEFT);
    }, Menu::ANIM_LEFT);
  }
} pausemenu_listener;

PauseMenu::PauseMenu() : Menu("resources/rml/pausemenu.rml"), open(false)
{
  document->GetElementById("options")->AddEventListener("click", &pausemenu_listener);
  document->GetElementById("exit")->AddEventListener("click", &pausemenu_listener);
}

void
PauseMenu::_hide()
{
  Camera::instance().enable(true);
  Menu::hide();
  OptionsMenu::instance().hide();
}

void
PauseMenu::_show()
{
  Camera::instance().enable(false);
  Menu::show();
}

void
PauseMenu::toggle()
{
  open ? _hide() : _show();
  open = !open;
}

PauseMenu&
PauseMenu::instance()
{
  static PauseMenu instance;
  return instance;
}

}
