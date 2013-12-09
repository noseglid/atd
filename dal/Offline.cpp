#include "dal/Offline.h"
#include "Exception.h"
#include "Hash.h"
#include "User.h"
#include "IO.h"
#include "Debug.h"

namespace dal {

using namespace soci;

Offline::Offline() : authenticated(false), sql(soci::sqlite3, "resources/offline.db")
{
}

void
Offline::get_user(
  std::string username,
  std::string password,
  std::function<void(bool success, struct user)> cb)
{
  try {
    user u;
    std::string dbpassword;
    sql << "SELECT username, password FROM users WHERE username = :username",
      use(username, "username"), into(u.username), into(dbpassword);

    bool success = sql.got_data() && (dbpassword == Hash::sha1(password));

    if (success) {
      authenticated = true;
      active_user = username;
    }

    cb(success, u);
  } catch (const soci_error& e) {
    DBGERR(e.what());
    cb(false, user());
  }
}

void
Offline::get_levels(std::function<void(bool success, struct levels)> cb)
{
  if (!authenticated) {
    DBGWRN("Not authenticated");
    cb(false, levels());
    return;
  }

  try {
    levels ret;
    rowset<row> rs = sql.prepare <<
      "SELECT id, spec FROM levels";

    for (rowset<row>::const_iterator it = rs.begin(); it != rs.end(); ++it) {
      dal::level level;
      *it >> level.id;
      level.spec = Json::deserialize(it->get<std::string>(1));
      ret.list.push_back(level);
    }

    cb(true, ret);
  } catch (const soci_error& e) {
    DBGERR(e.what());
    cb(false, levels());
  }
}

void
Offline::get_completed_levels(std::function<void(bool success, struct completed_levels)> cb)
{
  if (!authenticated) {
    DBGWRN("Not authenticated");
    cb(false, completed_levels());
    return;
  }

  try {
    completed_levels ret;
    rowset<row> rs = (sql.prepare <<
      "SELECT levelid FROM completed_levels WHERE username = :username",
      use(active_user, "username"));

    for (rowset<row>::const_iterator it = rs.begin(); it != rs.end(); ++it) {
      dal::level level;
      *it >> level.id;
      ret.list.push_back(level);
    }

    cb(true, ret);
  } catch (const soci_error& e) {
    DBGERR(e.what());
    cb(false, completed_levels());
  }
}

void
Offline::set_level_completed(int id, std::function<void(bool success)> cb)
{
  if (!authenticated) {
    DBGWRN("Not authenticated");
    cb(false);
    return;
  }

  try {
    sql << "DELETE FROM completed_levels WHERE username = :username AND levelid = :id",
      use(active_user, "username"), use(id, "id");

    sql << "INSERT INTO completed_levels (username, levelid) VALUES (:username, :id)",
      use(active_user, "username"), use(id, "id");

    cb(true);
  } catch (const soci_error& e) {
    DBGERR(e.what());
    cb(false);
  }
}

}
