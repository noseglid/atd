#include "ui/LevelDatabase.h"
#include "User.h"
#include "Game.h"
#include "Debug.h"
#include "Exception.h"
#include "IO.h"

namespace ui {

LevelDatabase::LevelDatabase() : Rocket::Controls::DataSource("levels")
{
  dal::get()->get_levels([this](bool success, struct dal::levels levels) {
    for (dal::level l : levels.list) {
      this->levels.push_back({ l.id, l.spec, false });
    }
  });

  this->set_completed_levels();

  User::instance().on("changed", std::bind(&LevelDatabase::set_completed_levels, this));
  Game::instance().on("stop",    std::bind(&LevelDatabase::set_completed_levels, this));
}

void
LevelDatabase::set_completed_levels()
{
  auto cb = [this](bool success, struct dal::completed_levels completed) {
    for (level_entry& l : this->levels) {
      l.completed = false;
      for (dal::level cl : completed.list) {
        if ((cl.id == l.id) && (l.completed = true)) break;
      }
    }

    /* Notify rocket so the view is updated */
    this->NotifyRowChange("list");
  };

  dal::get()->get_completed_levels(cb);
}

void
LevelDatabase::GetRow(
  Rocket::Core::StringList& row,
  const Rocket::Core::String& table,
  int row_index,
  const Rocket::Core::StringList& columns)
{
  level_entry level = this->levels[row_index];
  for (Rocket::Core::String col : columns) {
    if (col == "name") {
      Rocket::Core::String entry(64, "%s", level.spec["name"].asString().c_str());
      row.push_back(entry);
    } else if (col == "waves") {
      Json::Array arr = level.spec["waves"].asArray();
      Rocket::Core::String entry(8, "%d", arr.size());
      row.push_back(entry);
    } else if (col == "completed") {
      Rocket::Core::String entry(8, "%s", level.completed ? "true" : "false" );
      row.push_back(entry);
    }
  }
}

Json::Value
LevelDatabase::get_level(int row_index, int& id) const
{
  id = levels[row_index].id;
  return levels[row_index].spec;
}

int
LevelDatabase::GetNumRows(const Rocket::Core::String& table)
{
  return levels.size();
}

LevelDatabase&
LevelDatabase::instance()
{
  static LevelDatabase instance;
  return instance;
}

}
