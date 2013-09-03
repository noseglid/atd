#include "dal/Dal.h"
#include "dal/Offline.h"

#include <string>

namespace dal {

static DataAccessInterface *current = new Offline();

void
layer_offline()
{
  delete current;
  current = new Offline();
}

DataAccessInterface *
get() {
  return current;
}

}
