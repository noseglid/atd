#include "Exception.h"

class PlayerDeath : public Exception
{
public:
  PlayerDeath() : Exception("Player died") {}
};
