#include "ui/SetUserMenu.h"
#include "ui/TitleMenu.h"
#include "dal/Dal.h"
#include "User.h"
#include "Debug.h"

#include <Rocket/Controls.h>

B_NS_UI

class SetUserListener : public Rocket::Core::EventListener
{
  void ProcessEvent(Rocket::Core::Event& event)
  {
    auto goback_fn = []() {
      SetUserMenu::instance().hide(200, []() {
        TitleMenu::instance().show(200, Menu::ANIM_RIGHT);
        SetUserMenu::instance().evolve();
      }, Menu::ANIM_RIGHT);
    };

    auto el = event.GetCurrentElement();
    if (el->GetId() == "back") {
      goback_fn();
    }

    if (el->GetId() == "login") {
      el->SetInnerRML("Authenticating...");
      el->SetClass("link", false);
      std::string user = SetUserMenu::instance().get_form_value("username");
      std::string pass = SetUserMenu::instance().get_form_value("password");
      User::instance().login(user, pass, [el, goback_fn](bool success) {
        el->SetInnerRML("Login");
        el->SetClass("link", true);
        if (success) goback_fn();
        else SetUserMenu::instance().feedback("Invalid username or password.");
      });
    }

  }
} set_user_listener;

SetUserMenu::SetUserMenu() : Menu("resources/rml/setuser.rml")
{
  document->GetElementById("back")->AddEventListener("click", &set_user_listener);
  document->GetElementById("login")->AddEventListener("click", &set_user_listener);

  Mix_Music *mus = Audio::instance().load_music("menu.ogg");
  Audio::instance().play(mus);
}

std::string
SetUserMenu::get_form_value(const char* id) const
{
  using Rocket::Controls::ElementFormControl;
  ElementFormControl *el = dynamic_cast<ElementFormControl*>(document->GetElementById(id));

  return std::string(el->GetValue().CString());
}

void
SetUserMenu::evolve()
{
  feedback();

  document->GetElementById("back")->SetProperty("display", "block");
  document->GetElementById("setuser-title")->SetInnerRML("Change user");

  static_cast<Rocket::Controls::ElementFormControl*>
    (document->GetElementById("username")) ->SetValue("");
  static_cast<Rocket::Controls::ElementFormControl*>
    (document->GetElementById("password")) ->SetValue("");
}

SetUserMenu&
SetUserMenu::instance()
{
  static SetUserMenu instance;
  return instance;
}

E_NS_UI
