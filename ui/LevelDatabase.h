#ifndef __UI__LEVELDATABASE_H__
#define __UI__LEVELDATABASE_H__

#include "ui/UI.h"

#include <pjson.hpp>
#include <Rocket/Controls/DataSource.h>

B_NS_UI

class LevelDatabase : public Rocket::Controls::DataSource
{
  std::vector<Json::Value> levels;

  LevelDatabase();

public:
  void GetRow(
    Rocket::Core::StringList& row,
    const Rocket::Core::String& table,
    int row_index,
    const Rocket::Core::StringList& columns
  );

  int GetNumRows(const Rocket::Core::String& table);

  static LevelDatabase& instance();
};

E_NS_UI

#endif
