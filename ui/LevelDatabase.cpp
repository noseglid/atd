#include "ui/LevelDatabase.h"
#include "User.h"
#include "Debug.h"
#include "Exception.h"
#include "IO.h"

#include <dirent.h>

B_NS_UI

LevelDatabase::LevelDatabase() : Rocket::Controls::DataSource("levels")
{
  dal::get()->get_levels([this](bool success, struct dal::levels levels) {
    for (dal::level l : levels.list) {
      this->levels.push_back({ l.id, l.spec });
    }

    dal::get()->
      get_completed_levels([this](bool success,
                                  struct dal::completed_levels completed) {
      for (dal::level cl : completed.list) {
        for (level_entry& l : this->levels) {
          if (cl.id == l.id) {
            l.completed = true;
            break;
          }
        }
      }
    });
  });
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
      Rocket::Core::String entry(8, "%s", level.completed ? "Done" : "HELP!" );
      row.push_back(entry);
    }
  }
}

Json::Value
LevelDatabase::get_level(int row_index) const
{
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

E_NS_UI
