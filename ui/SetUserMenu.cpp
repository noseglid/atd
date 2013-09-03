#include "ui/SetUserMenu.h"
#include "ui/TitleMenu.h"
#include "dal/Dal.h"

B_NS_UI

class SetUserListener : public Rocket::Core::EventListener
{
  void ProcessEvent(Rocket::Core::Event& event)
  {
    auto el = event.GetCurrentElement();
    if (el->GetId() == "back") {
      /* Nothing to see here */
    }

    if (el->GetId() == "offline") {
      dal::layer_offline();
    }

    /* Should always be done.. for now atleast */
    SetUserMenu::instance().hide(200, []() {
      TitleMenu::instance().show(200, Menu::ANIM_RIGHT);
    }, Menu::ANIM_RIGHT);
  }
} set_user_listener;

SetUserMenu::SetUserMenu() : Menu("resources/rml/setuser.rml")
{
  document->GetElementById("back")->AddEventListener("click", &set_user_listener);
  document->GetElementById("offline")->AddEventListener("click", &set_user_listener);
}

SetUserMenu&
SetUserMenu::instance()
{
  static SetUserMenu instance;
  return instance;
}

E_NS_UI
