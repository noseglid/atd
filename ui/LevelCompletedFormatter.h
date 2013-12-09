#ifndef __UI__LEVELCOMPLETEDFORMATTER_H__
#define __UI__LEVELCOMPLETEDFORMATTER_H__

#include "ui/UI.h"
#include <Rocket/Controls/DataFormatter.h>

namespace ui {

class LevelCompletedFormatter : public Rocket::Controls::DataFormatter
{
  LevelCompletedFormatter();
  ~LevelCompletedFormatter();

  LevelCompletedFormatter(const LevelCompletedFormatter&);
  void operator=(LevelCompletedFormatter const&);

public:

  void FormatData(Rocket::Core::String& formatted, const Rocket::Core::StringList& raw);
  static LevelCompletedFormatter& instance();
};

}

#endif
