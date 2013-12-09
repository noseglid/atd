#include "ui/ui.h"
#include "ui/System.h"
#include "ui/Renderer.h"
#include "ui/LevelCompletedFormatter.h"
#include "RocketSDL.h"
#include "engine/Video.h"
#include "gl/Transform.h"
#include "Exception.h"
#include "Debug.h"

#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>

namespace ui {

UI::UI()
{
  system = std::unique_ptr<System>(new System());
  renderer = std::unique_ptr<Renderer>(new Renderer());

  Rocket::Core::SetRenderInterface(renderer.get());
  Rocket::Core::SetSystemInterface(system.get());

  Rocket::Core::Initialise();

  load_fonts();

  Rocket::Controls::Initialise();

  engine::resolution res = engine::Video::instance().get_resolution();

  context = Rocket::Core::CreateContext("default", Rocket::Core::Vector2i(res.width, res.height));

  if (getenv("ROCKETDEBUG")) {
    Rocket::Debugger::Initialise(context);
    Rocket::Debugger::SetVisible(true);
  }

  engine::Engine& e = engine::Engine::instance();
  e.on("tick_nodepth", std::bind(&UI::tick,        this, std::placeholders::_1));
  e.on("keydown",      std::bind(&UI::key,         this, true,  std::placeholders::_1));
  e.on("keyup",        std::bind(&UI::key,         this, false, std::placeholders::_1));
  e.on("mousemotion",  std::bind(&UI::mousemotion, this, std::placeholders::_1));
  e.on("mousedown",    std::bind(&UI::mousedown,   this, std::placeholders::_1));
  e.on("mouseup",      std::bind(&UI::mouseup,     this, std::placeholders::_1));

  /* Make sure this instantiated before Rocket starts doing stuff */
  ui::LevelCompletedFormatter::instance();
}

void
UI::load_fonts()
{
  Rocket::Core::FontDatabase::LoadFontFace(
    "resources/fonts/Riky-Vampdator.ttf",
    "Riky",
    Rocket::Core::Font::Style::STYLE_NORMAL,
    Rocket::Core::Font::Weight::WEIGHT_NORMAL
  );
}

void
UI::tick(const engine::Event& ev)
{
  glPushMatrix();
    gl::Transform::enable2D(true);
    context->Update();
    context->Render();
    gl::Transform::disable2D();
  glPopMatrix();
}

void
UI::key(bool down, const engine::Event& ev)
{
  using Rocket::Core::Context;

  (down ?
    std::bind(&Context::ProcessKeyDown, context, std::placeholders::_1, std::placeholders::_2) :
    std::bind(&Context::ProcessKeyUp,   context, std::placeholders::_1, std::placeholders::_2)
  )(
    RocketSDL::sdl2rocket_key(ev.ev.key),
    RocketSDL::sdl2rocket_modifier(SDL_GetModState())
  );

  /*
   * sym.unicode & 0xFF80 - if this is 0, then it is an ascii character
   * sym.unicode & 0x7F   - The actual ascii value, only take printables
   * sym.unicode != 0x7F  - do not process delete (backspace?)
   */
  SDL_keysym sym = ev.ev.key.keysym;
  if ((sym.unicode & 0xFF80) == 0 && (sym.unicode & 0x7F) >= 32 && sym.unicode != 0x7F) {
    wchar_t c = static_cast<wchar_t>(ev.ev.key.keysym.unicode);
    context->ProcessTextInput(c);
  }
}

void
UI::mousemotion(const engine::Event& ev)
{
  context->ProcessMouseMove(
    ev.ev.motion.x,
    ev.ev.motion.y,
    RocketSDL::sdl2rocket_modifier(SDL_GetModState())
  );
}

void
UI::mousedown(const engine::Event& ev)
{
  if (ev.ev.button.button == SDL_BUTTON_WHEELUP ||
      ev.ev.button.button == SDL_BUTTON_WHEELDOWN) {
    context->ProcessMouseWheel(
      ev.ev.button.button == SDL_BUTTON_WHEELUP ? -1 : 1,
      RocketSDL::sdl2rocket_modifier(SDL_GetModState())
    );
  } else {
    context->ProcessMouseButtonDown(
      RocketSDL::sdl2rocket_button_index(ev.ev.button),
      RocketSDL::sdl2rocket_modifier(SDL_GetModState())
    );
  }
}

void
UI::mouseup(const engine::Event& ev)
{
  context->ProcessMouseButtonUp(
    RocketSDL::sdl2rocket_button_index(ev.ev.button),
    RocketSDL::sdl2rocket_modifier(SDL_GetModState())
  );
}

UI&
UI::instance()
{
  static UI instance;
  return instance;
}

Rocket::Core::ElementDocument *
UI::load(const std::string& resource)
{
  Rocket::Core::ElementDocument *d;
  if (NULL == (d = context->LoadDocument(resource.c_str()))) {
    throw Exception("Could not load rml from: " + resource);
  }

  return d;
}

void
UI::res_change(int width, int height)
{
  context->SetDimensions(Rocket::Core::Vector2i(width, height));
  load_fonts();
}

}
