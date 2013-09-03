#include "ui/LevelDatabase.h"
#include "dal/Dal.h"
#include "Debug.h"
#include "Exception.h"
#include "IO.h"

#include <dirent.h>

B_NS_UI

LevelDatabase::LevelDatabase() : Rocket::Controls::DataSource("levels")
{
  dal::get()->get_levels([this](struct dal::levels levels) {
    this->levels = levels.specs;
  });
}

void
LevelDatabase::GetRow(
  Rocket::Core::StringList& row,
  const Rocket::Core::String& table,
  int row_index,
  const Rocket::Core::StringList& columns)
{
  Json::Value spec = levels[row_index];
  for (Rocket::Core::String col : columns) {
    if (col == "name") {
      Rocket::Core::String entry(64, "%s", spec["name"].asString().c_str());
      row.push_back(entry);
    } else if (col == "waves") {
      Json::Array arr = spec["waves"].asArray();
      Rocket::Core::String entry(8, "%d", arr.size());
      row.push_back(entry);
    }
  }
}

Json::Value
LevelDatabase::get_level(int row_index) const
{
  return levels[row_index];
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
