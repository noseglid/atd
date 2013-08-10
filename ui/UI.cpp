#include "ui/ui.h"
#include "ui/System.h"
#include "ui/Renderer.h"
#include "engine/Video.h"
#include "GLTransform.h"
#include "Exception.h"
#include "Debug.h"
#include "ui/LevelSelectMenu.h"

#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>

B_NS_UI

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

#if ROCKETDEBUG == 1
  Rocket::Debugger::Initialise(context);
  Rocket::Debugger::SetVisible(true);
#endif

  engine::Engine& e = engine::Engine::instance();
  e.on("tick_nodepth", std::bind(&UI::tick,        this, std::placeholders::_1));
  e.on("mousemotion",  std::bind(&UI::mousemotion, this, std::placeholders::_1));
  e.on("mousedown",    std::bind(&UI::mousedown,   this, std::placeholders::_1));
  e.on("mouseup",      std::bind(&UI::mouseup,     this, std::placeholders::_1));
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
    GLTransform::enable2D(true);
    context->Update();
    context->Render();
    GLTransform::disable2D();
  glPopMatrix();
}

void
UI::mousemotion(const engine::Event& ev)
{
  context->ProcessMouseMove(ev.ev.motion.x, ev.ev.motion.y, 0);
}

void
UI::mousedown(const engine::Event& ev)
{
  context->ProcessMouseButtonDown(0, 0);
}

void
UI::mouseup(const engine::Event& ev)
{
  context->ProcessMouseButtonUp(0, 0);
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

E_NS_UI
