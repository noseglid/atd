#ifndef __DAL__OFFLINE_H__
#define __DAL__OFFLINE_H__

#include "dal/Dal.h"
#include "dal/DataAccessInterface.h"

#include <sqlite3.h>

B_NS_DAL

class Offline : public DataAccessInterface
{
  sqlite3 *db;

public:
  Offline();
  ~Offline();

  std::string get_default_user();

  void get_user(
    std::string username,
    std::string password,
    std::function<void(bool success, struct user)> cb
  );

  void get_levels(
    std::function<void(struct levels)> cb
  );

  void get_completed_levels(
    std::function<void(struct completed_levels)> cb
  );
};

E_NS_DAL

#endif
