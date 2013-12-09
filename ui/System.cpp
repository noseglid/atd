#include "ui/System.h"
#include "Debug.h"

namespace ui {

float
System::GetElapsedTime()
{
  return SDL_GetTicks() / 1000.0f;
}

bool
System::LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String& message)
{
  using Rocket::Core::Log;
  switch (type) {
    case Log::LT_ERROR:   DBGERR(message.CString()); break;
    case Log::LT_ASSERT:  DBGERR(message.CString()); break;
    case Log::LT_WARNING: DBGWRN(message.CString()); break;
    case Log::LT_INFO:    DBG   (message.CString()); break;
    case Log::LT_DEBUG:   DBG   (message.CString()); break;
    case Log::LT_ALWAYS:  DBG   (message.CString()); break;
    case Log::LT_MAX:     DBG   (message.CString()); break;
  }

  return true;
}

}
