#include "ui/SetUserMenu.h"
#include "ui/TitleMenu.h"
#include "dal/Dal.h"
#include "User.h"
#include "Debug.h"

#include <Rocket/Controls.h>

namespace ui {

class SetUserListener : public Rocket::Core::EventListener
{
  void ProcessEvent(Rocket::Core::Event& event)
  {
    SetUserMenu::instance().hide(200, []() {
      TitleMenu::instance().show(200, Menu::ANIM_RIGHT);
      SetUserMenu::instance().evolve();
    }, Menu::ANIM_RIGHT);
  }
} set_user_back_listener;

class SubmitListener : public Rocket::Core::EventListener
{
  void ProcessEvent(Rocket::Core::Event& event)
  {
    SetUserMenu::instance().feedback("Authenticating...", false);
    auto el = event.GetCurrentElement();

    std::string user(event.GetParameter<Rocket::Core::String>("username", "").CString());
    std::string pass(event.GetParameter<Rocket::Core::String>("password", "").CString());
    User::instance().login(user, pass, [el](bool success) {
      if (success) {
        SetUserMenu::instance().hide(200, []() {
          TitleMenu::instance().show(200, Menu::ANIM_RIGHT);
          SetUserMenu::instance().evolve();
        }, Menu::ANIM_RIGHT);
        return;
      }

      SetUserMenu::instance().feedback("Invalid username or password.");
    });
  }
} submit_listener;

SetUserMenu::SetUserMenu() : Menu("resources/rml/setuser.rml")
{
  document->GetElementById("back")->AddEventListener("click", &set_user_back_listener);
  document->GetElementById("user-form")->AddEventListener("submit", &submit_listener);

  engine::Engine::instance().on(
    "keyup", std::bind(&SetUserMenu::keyup, this, std::placeholders::_1)
  );

  Mix_Music *mus = Audio::instance().load_music("menu.ogg");
  Audio::instance().play(mus);

  feedback("Please enter credentials", false);
}

void
SetUserMenu::keyup(const engine::Event& ev)
{
  if (ev.ev.key.keysym.sym != SDLK_RETURN) return;

  static_cast<Rocket::Controls::ElementForm*>
    (document->GetElementById("user-form"))->Submit();
}

void
SetUserMenu::evolve()
{
  feedback("Please enter credentials", false);

  document->GetElementById("back")->SetProperty("display", "block");
  document->GetElementById("setuser-title")->SetInnerRML("Change user");

  static_cast<Rocket::Controls::ElementFormControl*>
    (document->GetElementById("username"))->SetValue("");
  static_cast<Rocket::Controls::ElementFormControl*>
    (document->GetElementById("password"))->SetValue("");
}

SetUserMenu&
SetUserMenu::instance()
{
  static SetUserMenu instance;
  return instance;
}

}
