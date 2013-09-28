#ifndef __USER_H__
#define __USER_H__

#include <de.h>

class User : public de::Emitter<>
{
  std::string username;

  User();
  User(const User& rhs);
  void operator=(const User& rhs);

public:
  static User& instance();

  bool has_user() const;
  std::string get_username() const;

  void login(std::string user, std::string pass, std::function<void(bool)> cb);
};

#endif
