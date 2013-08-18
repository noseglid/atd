#include "ui/LevelSelectMenu.h"
#include "ui/TitleMenu.h"
#include "ui/LevelDatabase.h"
#include "Debug.h"
#include "Game.h"

#include <Rocket/Controls/ElementDataGrid.h>
#include <Rocket/Controls/ElementDataGridRow.h>

B_NS_UI

class BackListener : public Rocket::Core::EventListener
{
  void ProcessEvent(Rocket::Core::Event& event)
  {
    LevelSelectMenu::instance().hide(200, []() {
      TitleMenu::instance().show(200, []() {}, Menu::ANIM_RIGHT);
    }, Menu::ANIM_RIGHT);
  }
} back_listener;

class SelectListener : public Rocket::Core::EventListener
{
  void ProcessEvent(Rocket::Core::Event& event)
  {
    if (!LevelSelectMenu::instance().is_visible()) return;

    Rocket::Controls::ElementDataGrid *el =
      dynamic_cast<Rocket::Controls::ElementDataGrid*>(event.GetCurrentElement());
    el->RemoveEventListener("click", this);

    TitleMenu::instance().display(false);
    LevelSelectMenu::instance().hide(0, [el, this](){
      el->AddEventListener("click", this);
    }, Menu::ANIM_LEFT);

    int click_index = 0;
    for (; click_index < el->GetNumRows(); ++click_index) {
      Rocket::Controls::ElementDataGridRow *row = el->GetRow(click_index);
      if (row->IsPseudoClassSet("active")) break;
    }

    Game::instance().start(LevelDatabase::instance().get_level(click_index));
  }
} select_listener;

LevelSelectMenu::LevelSelectMenu() : Menu("resources/rml/levelselect.rml")
{
  LevelDatabase::instance();
  document->GetElementById("back")->AddEventListener("click", &back_listener);
  document->GetElementById("levellist")->AddEventListener("click", &select_listener);
}

LevelSelectMenu&
LevelSelectMenu::instance()
{
  static LevelSelectMenu instance;
  return instance;
}

E_NS_UI
