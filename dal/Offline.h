#ifndef __DAL__OFFLINE_H__
#define __DAL__OFFLINE_H__

#include "dal/Dal.h"
#include "dal/DataAccessInterface.h"

#include <soci.h>
#include <sqlite3/soci-sqlite3.h>

B_NS_DAL

class Offline : public DataAccessInterface
{
  bool authenticated;
  std::string active_user;
  soci::session sql;

public:
  Offline();

  void get_user(
    std::string username,
    std::string password,
    std::function<void(bool success, struct user)> cb
  );

  void get_levels(
    std::function<void(bool success, struct levels)> cb
  );

  void get_completed_levels(
    std::function<void(bool success, struct completed_levels)> cb
  );
};

E_NS_DAL

#endif
