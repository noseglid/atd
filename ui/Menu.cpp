#include "Menu.h"
#include "Debug.h"
#include "engine/Video.h"

#include <math.h>

B_NS_UI

Menu::Menu(std::string docpath) :
  anim_pending(false),
  sfx_change_menu(Audio::instance().load_sfx("swish1.ogg")),
  document(UI::instance().load(docpath))
{
  engine::Engine::instance().on("tick", std::bind(&Menu::tick, this, std::placeholders::_1));
  animate = document->GetElementById("main");
  animate->SetProperty("left", Rocket::Core::Property(999999, Rocket::Core::Property::NUMBER));
  display();
}

void
Menu::show()
{
  show(0, ANIM_LEFT);
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
  hide(0, ANIM_LEFT);
}

void
Menu::hide(int ms, ANIMDIR dir)
{
  hide(ms, [](){}, dir);
}

void
Menu::hide(int ms, std::function<void()> ondone, ANIMDIR dir)
{
  if (anim_pending) {
    DBGWRN("Tried to start a new animation while ongoing.");
    return;
  }

  Audio::instance().play(sfx_change_menu);
  anim_pending    = true;
  anim_duration   = ms / 1000.0f;
  anim_time_start = SDL_GetTicks() / 1000.0f;
  anim_pos_start  = 0;
  anim_pos_end    = engine::Video::instance().get_resolution().width * (dir == ANIM_LEFT ? -1 : 1);
  anim_done       = ondone;
}

void
Menu::toggle()
{
  is_visible() ? hide() : show();
}

bool
Menu::is_visible() const
{
  int width = engine::Video::instance().get_resolution().width;
  float left = animate->GetAbsoluteLeft();
  return !(left < 0 || left > width);
}

void
Menu::display(bool visible)
{
  visible ? document->Show() : document->Hide();
}

void
Menu::tick(const engine::Event& ev)
{
  if (!anim_pending) return;

  float time    = SDL_GetTicks() / 1000.0f;
  float elapsed = time - anim_time_start;

  float factor  = std::min(elapsed / anim_duration, 1.0f);
  /* if anim_duration is close to 0.0, factor might be nan (division by ~zero) */
  if (isnan(factor)) factor = 1.0f;

  int left      = anim_pos_start - factor * (anim_pos_start - anim_pos_end);

  animate->SetProperty("left", Rocket::Core::Property(left, Rocket::Core::Property::NUMBER));
  if (elapsed >= anim_duration) {
    anim_pending = false;
    anim_done();
  }
}

E_NS_UI
