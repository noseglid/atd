#ifndef __UI__TITLEMENU_H__
#define __UI__TITLEMENU_H__

#include "engine/Engine.h"
#include "ui/UI.h"
#include "ui/System.h"
#include "ui/Renderer.h"

#include <de.h>

B_NS_UI

class TitleMenu
{
  Rocket::Core::ElementDocument *document;

  TitleMenu();
  TitleMenu(const TitleMenu& rhs);
  void operator=(const TitleMenu& rhs);
  void tick(const engine::Event& ev);

  void mousemotion(const engine::Event& ev);
  void mousedown(const engine::Event& ev);
  void mouseup(const engine::Event& ev);

public:

  static TitleMenu& instance();
  void hide();
};

E_NS_UI

#endif
