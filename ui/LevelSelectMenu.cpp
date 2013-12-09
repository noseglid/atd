#include "ui/LevelSelectMenu.h"
#include "ui/TitleMenu.h"
#include "ui/LevelDatabase.h"
#include "Debug.h"
#include "Game.h"

#include <Rocket/Controls/ElementDataGrid.h>
#include <Rocket/Controls/ElementDataGridRow.h>

namespace ui {

class BackListener : public Rocket::Core::EventListener
{
  void ProcessEvent(Rocket::Core::Event& event)
  {
    LevelSelectMenu::instance().hide(200, []() {
      TitleMenu::instance().show(200, []() {}, Menu::ANIM_RIGHT);
    }, Menu::ANIM_RIGHT);
  }
} level_select_back_listener;

class SelectListener : public Rocket::Core::EventListener
{
  bool processing;

public:
  SelectListener() : processing(false) {}

  void ProcessEvent(Rocket::Core::Event& event)
  {
    if (processing || !LevelSelectMenu::instance().is_visible()) return;
    processing = true;

    Rocket::Controls::ElementDataGrid *el =
      dynamic_cast<Rocket::Controls::ElementDataGrid*>(event.GetCurrentElement());

    int click_index = 0;
    for (; click_index < el->GetNumRows(); ++click_index) {
      Rocket::Controls::ElementDataGridRow *row = el->GetRow(click_index);
      if (row->IsPseudoClassSet("active")) break;
    }

    if (click_index >= el->GetNumRows()) {
      return;
    }

    TitleMenu::instance().display(false);
    LevelSelectMenu::instance().hide(0, [this]() {
      processing = false;
    }, Menu::ANIM_LEFT);

    int id = 0;
    Json::Value spec = LevelDatabase::instance().get_level(click_index, id);
    Game::instance().start(id, spec);
  }
} select_listener;

LevelSelectMenu::LevelSelectMenu() : Menu("resources/rml/levelselect.rml")
{
  LevelDatabase::instance();
  document->GetElementById("back")->AddEventListener("click", &level_select_back_listener);
  document->GetElementById("levellist")->AddEventListener("click", &select_listener);
}

LevelSelectMenu&
LevelSelectMenu::instance()
{
  static LevelSelectMenu instance;
  return instance;
}

}
