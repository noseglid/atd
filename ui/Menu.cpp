#include "Menu.h"
#include "Debug.h"
#include "engine/Video.h"

B_NS_UI

Menu::Menu(std::string docpath) :
  anim_pending(false),
  sfx_change_menu(Audio::instance().load_sfx("swish1.ogg")),
  document(UI::instance().load(docpath))
{
  engine::Engine::instance().on("tick", std::bind(&Menu::tick, this, std::placeholders::_1));
}

void
Menu::show()
{
  document->Show();
}

void
Menu::show(int ms, ANIMDIR dir)
{
  show(ms, [](){}, dir);
}

void
Menu::show(int ms, std::function<void()> ondone, ANIMDIR dir)
{
  if (anim_pending) {
    DBGWRN("Tried to start a new animation while ongoing.");
    return;
  }

  /* Make sure it's far away so it wont show before we animate it */
  document->SetProperty("left", Rocket::Core::Property(999999, Rocket::Core::Property::NUMBER));

  this->show();
  anim_pending    = true;
  anim_duration   = ms / 1000.0f;
  anim_time_start = SDL_GetTicks() / 1000.0f;
  anim_pos_start  = engine::Video::instance().get_resolution().width * (dir == ANIM_LEFT ? 1 : -1);
  anim_pos_end    = 0;
  anim_done       = ondone;
}

void
Menu::hide()
{
  document->Hide();
}

void
Menu::hide(int ms, std::function<void()> ondone, ANIMDIR dir)
{
  if (anim_pending) {
    DBGWRN("Tried to start a new animation while ongoing.");
    return;
  }

  document->SetProperty("left", Rocket::Core::Property(0, Rocket::Core::Property::NUMBER));
  Audio::instance().play(sfx_change_menu);
  anim_pending    = true;
  anim_duration   = ms / 1000.0f;
  anim_time_start = SDL_GetTicks() / 1000.0f;
  anim_pos_start  = 0;
  anim_pos_end    = engine::Video::instance().get_resolution().width * (dir == ANIM_LEFT ? -1 : 1);
  anim_done       = [this, ondone]() { ondone(); };
}

void
Menu::tick(const engine::Event& ev)
{
  if (!anim_pending) return;

  float time    = SDL_GetTicks() / 1000.0f;
  float elapsed = time - anim_time_start;
  float factor  = std::min(elapsed / anim_duration, 1.0f);
  int left      = anim_pos_start - factor * (anim_pos_start - anim_pos_end);

  document->SetProperty("left", Rocket::Core::Property(left, Rocket::Core::Property::NUMBER));
  if (elapsed >= anim_duration) {
    anim_pending = false;
    anim_done();
  }
}

E_NS_UI
