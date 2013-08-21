#include "ui/OptionsMenu.h"
#include "ui/TitleMenu.h"
#include "ui/PauseMenu.h"
#include "Game.h"
#include "Debug.h"

B_NS_UI

class OptionsMenuListener : public Rocket::Core::EventListener
{
  void ProcessEvent(Rocket::Core::Event& event)
  {
    OptionsMenu::instance().hide(200, []() {
        OptionsMenu::instance().get_back()->show(200, []() {}, Menu::ANIM_RIGHT);
    }, Menu::ANIM_RIGHT);
  }
} back_listener;

class VolumeListener : public Rocket::Core::EventListener
{
  void ProcessEvent(Rocket::Core::Event& event)
  {
    float factor = event.GetParameter<float>("value", 0.0);
    auto el = event.GetCurrentElement();
    if (el->GetId() == "vol-sfx") {
      Audio::instance().set_sfx_volume(MIX_MAX_VOLUME * factor);
    }

    if (el->GetId() == "vol-music") {
      Audio::instance().set_music_volume(MIX_MAX_VOLUME * factor);
    }
  }
} vol_listener;

OptionsMenu::OptionsMenu() : Menu("resources/rml/options.rml")
{
  back = &TitleMenu::instance();
  document->GetElementById("back")->AddEventListener("click", &back_listener);
  document->GetElementById("vol-sfx")->AddEventListener("change", &vol_listener);
  document->GetElementById("vol-music")->AddEventListener("change", &vol_listener);

  Game::instance().on("start", [this]() { this->back = &PauseMenu::instance(); });
  Game::instance().on("stop",  [this]() { this->back = &TitleMenu::instance(); });
}

Menu *
OptionsMenu::get_back() const
{
  return this->back;
}

OptionsMenu&
OptionsMenu::instance()
{
  static OptionsMenu instance;
  return instance;
}

E_NS_UI
