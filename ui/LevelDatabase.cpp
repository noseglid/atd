#include "ui/LevelDatabase.h"
#include "Debug.h"
#include "Exception.h"
#include "IO.h"

#include <dirent.h>

B_NS_UI

LevelDatabase::LevelDatabase() : Rocket::Controls::DataSource("levels")
{
  DIR *dir;
  struct dirent *ent;
  if (NULL == (dir = opendir("resources/levels/"))) {
    throw Exception("Could not list levels-folder.");
  }

  while (NULL != (ent = readdir(dir))) {
    if (0 == strncmp(".", ent->d_name, 1) || 0 == strcmp("..", ent->d_name)) continue;
    std::string file = "resources/levels/" + std::string(ent->d_name);
    Json::Value spec = Json::deserialize(IO::file_get_contents(file));
    levels.push_back(spec);
  }

  closedir(dir);
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
