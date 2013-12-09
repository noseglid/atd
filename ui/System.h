#ifndef __UI__SYSTEM_H__
#define __UI__SYSTEM_H__

#include "ui/UI.h"

#include <SDL.h>
#include <Rocket/Core/SystemInterface.h>

namespace ui {

class System : public Rocket::Core::SystemInterface
{
public:
  float GetElapsedTime();
  bool LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String& message);
};

}

#endif
