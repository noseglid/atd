#ifndef __DAL__OFFLINE_H__
#define __DAL__OFFLINE_H__

#include "dal/DataAccessInterface.h"

#include <sqlite3.h>

namespace dal {

class Offline : public DataAccessInterface
{
  sqlite3 *db;

public:
  Offline();
  ~Offline();

  void get_user(
    std::string user,
    std::function<void(struct user)> cb
  );

  void get_levels(
    std::function<void(struct levels)> cb
  );

  void get_completed_levels(
    std::string user,
    std::function<void(struct completed_levels)> cb
  );
};

}

#endif
