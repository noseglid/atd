#ifndef __DAL__DATAACCESSINTERFACE_H__
#define __DAL__DATAACCESSINTERFACE_H__

#include <string>
#include <vector>
#include <functional>
#include <pjson.hpp>

B_NS_DAL

struct level
{
  /**
   *  Server-side id of this level.
   */
  int id;

  /**
   *  The JSON spec of this level, may not be set.
   */
  Json::Value spec;
};

struct user
{
  /**
   *  User's username
   */
  std::string username;
};

struct levels
{
  /**
   *  List of all levels
   */
  std::vector<level> list;
};

struct completed_levels
{
  /**
   *  List of completed levels
   */
  std::vector<level> list;
};

class DataAccessInterface
{
public:
  virtual ~DataAccessInterface() {}

  virtual void get_user(
    std::string username,
    std::string password,
    std::function<void(bool success, struct user)> cb
  ) = 0;

  virtual void get_levels(
    std::function<void(bool success, struct levels)> cb
  ) = 0;

  virtual void get_completed_levels(
    std::function<void(bool success, struct completed_levels)> cb
  ) = 0;
};

E_NS_DAL

#endif
