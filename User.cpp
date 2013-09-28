#include "User.h"
#include "Debug.h"
#include "dal/dal.h"

User::User()
{
}

bool
User::has_user() const
{
  return (!username.empty());
}

std::string
User::get_username() const
{
  return username;
}

void
User::login(std::string username, std::string password, std::function<void(bool)> cb)
{
  dal::get()->get_user(username, password, [this, cb](bool success, dal::user user) {
    if (!success) {
      cb(success);
      return;
    }

    this->username = user.username;
    cb(success);
    this->emit("changed");
  });
}

User&
User::instance()
{
  static User instance;
  return instance;
}
