#include "ui/TitleMenu.h"
#include "ui/OptionsMenu.h"
#include "Debug.h"

B_NS_UI

class Listener : public Rocket::Core::EventListener
{
  void ProcessEvent(Rocket::Core::Event& event)
  {
    OptionsMenu::instance().hide(200, []() {
      TitleMenu::instance().show(200, []() {}, Menu::ANIM_RIGHT);
    }, Menu::ANIM_RIGHT);
  }
} listener;

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
  document->GetElementById("back")->AddEventListener("click", &listener);
  document->GetElementById("vol-sfx")->AddEventListener("change", &vol_listener);
  document->GetElementById("vol-music")->AddEventListener("change", &vol_listener);
}

OptionsMenu&
OptionsMenu::instance()
{
  static OptionsMenu instance;
  return instance;
}

E_NS_UI
