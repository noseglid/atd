#include "ui/LevelCompletedFormatter.h"

B_NS_UI

LevelCompletedFormatter::LevelCompletedFormatter() :
  Rocket::Controls::DataFormatter("levelCompleted")
{
}

LevelCompletedFormatter::~LevelCompletedFormatter()
{
}

void
LevelCompletedFormatter::FormatData(
  Rocket::Core::String& formatted,
  const Rocket::Core::StringList& raw)
{
  formatted = (raw[0] == "true") ?
    "<state class=\"ok\" />" :
    "<state class=\"nok\" />";
}

LevelCompletedFormatter&
LevelCompletedFormatter::instance()
{
  static LevelCompletedFormatter instance;
  return instance;
}

E_NS_UI
