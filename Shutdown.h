#ifndef __SHUTDOWN_H__
#define __SHUTDOWN_H__

#include "Exception.h"

class Shutdown : public Exception
{
public:
  Shutdown(std::string msg) : Exception(msg) {}
};

#endif
