#include "ui/SetUserMenu.h"
#include "ui/TitleMenu.h"
#include "dal/Dal.h"
#include "Debug.h"

#include <Rocket/Controls.h>

B_NS_UI

class SetUserListener : public Rocket::Core::EventListener
{
  void ProcessEvent(Rocket::Core::Event& event)
  {
    auto el = event.GetCurrentElement();
    if (el->GetId() == "back") {
      /* Nothing to see here */
    }

    if (el->GetId() == "login") {
      /* Allrigth, do the actual login */
    }

    SetUserMenu::instance().hide(200, []() {
      TitleMenu::instance().show(200, Menu::ANIM_RIGHT);
    }, Menu::ANIM_RIGHT);
  }
} set_user_listener;

SetUserMenu::SetUserMenu() : Menu("resources/rml/setuser.rml")
{
  document->GetElementById("back")->AddEventListener("click", &set_user_listener);
  document->GetElementById("login")->AddEventListener("click", &set_user_listener);
}

std::string
SetUserMenu::get_form_value(const char* id) const
{
  using Rocket::Controls::ElementFormControl;
  ElementFormControl *el = dynamic_cast<ElementFormControl*>(document->GetElementById(id));

  return std::string(el->GetValue().CString());
}

SetUserMenu&
SetUserMenu::instance()
{
  static SetUserMenu instance;
  return instance;
}

E_NS_UI
