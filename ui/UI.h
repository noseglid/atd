#ifndef __UI__UI_H_
#define __UI__UI_H_

#define B_NS_UI namespace ui {
#define E_NS_UI }

#include "engine/Engine.h"

#include <Rocket/Core.h>

B_NS_UI

class System;
class Renderer;

class UI
{
  /* unique_ptr used to avoid circular dependecy with system/renderer */
  std::unique_ptr<System> system;
  std::unique_ptr<Renderer> renderer;

  Rocket::Core::Context *context;

  UI();
  void load_fonts();

  void tick(const engine::Event& ev);
  void key(bool down, const engine::Event& ev);
  void mousemotion(const engine::Event& ev);
  void mousedown(const engine::Event& ev);
  void mouseup(const engine::Event& ev);

public:
  static UI& instance();

  Rocket::Core::ElementDocument *load(const std::string& resouce);
  void res_change(int width, int height);
};

E_NS_UI

#endif
