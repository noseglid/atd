#ifndef __UI__LEVELDATABASE_H__
#define __UI__LEVELDATABASE_H__

#include "ui/UI.h"
#include "dal/Dal.h"

#include <pjson.hpp>
#include <Rocket/Controls/DataSource.h>

B_NS_UI

class LevelDatabase : public Rocket::Controls::DataSource
{
  struct level_entry {
    int id;
    Json::Value spec;
    bool completed;
  };

  std::vector<level_entry> levels;

  LevelDatabase();

public:
  void set_completed_levels();

  void GetRow(
    Rocket::Core::StringList& row,
    const Rocket::Core::String& table,
    int row_index,
    const Rocket::Core::StringList& columns
  );
  Json::Value get_level(int row_index, int& id) const;
  int GetNumRows(const Rocket::Core::String& table);

  static LevelDatabase& instance();
};

E_NS_UI

#endif
