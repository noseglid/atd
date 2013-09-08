#ifndef __DAL__DATAACCESSINTERFACE_H__
#define __DAL__DATAACCESSINTERFACE_H__

#include <string>
#include <vector>
#include <functional>
#include <pjson.hpp>

namespace dal {

struct user
{
  std::string username;
};

struct levels
{
  std::vector<Json::Value> specs;
};

struct completed_levels
{
  std::vector<std::string> level_names;
};

class DataAccessInterface
{
public:
  virtual ~DataAccessInterface() {}

  virtual void get_user(
    std::string username,
    std::function<void(struct user)> cb
  ) = 0;

  virtual void get_levels(
    std::function<void(struct levels)> cb
  ) = 0;

  virtual void get_completed_levels(
    std::string user,
    std::function<void(struct completed_levels)> cb
  ) = 0;
};

}

#endif
