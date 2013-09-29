#include "dal/Offline.h"
#include "Exception.h"
#include "Hash.h"
#include "User.h"
#include "IO.h"
#include "Debug.h"

B_NS_DAL

Offline::Offline()
{
  DBG("Data Access Layer: offline set");
  if (SQLITE_OK != sqlite3_open("resources/offline.db", &db)) {
    sqlite3_close(db);
    throw Exception("Could not open database.");
  }
}

Offline::~Offline()
{
  if (SQLITE_OK != sqlite3_close(db)) {
    DBGWRN("Could not close sqlite3 database connection");
  }
}

void
Offline::get_user(
  std::string username,
  std::string password,
  std::function<void(bool success, struct user)> cb)
{
  bool success = false;
  user u;
  sqlite3_stmt *stmt;
  const char *query = "SELECT * FROM users WHERE username = ?";

  if (SQLITE_OK != sqlite3_prepare_v2(db, query, -1, &stmt, NULL)) {
    throw Exception("Could not prepare statement.");
  }

  if (SQLITE_OK != sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC)) {
    throw Exception("Could not bind data to statement.");
  }

  if (SQLITE_ROW == sqlite3_step(stmt)) {
    u.username =
      std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    std::string dbpw =
      std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));

    success = (dbpw == Hash::sha1(password));
  }

  cb(success, u);
}

int
level_db_row(void *ctx, int ncols, char **values, char **colnames)
{
  dal::level level;
  level.id   = atoi(values[0]);
  level.spec = Json::deserialize(values[1]);

  static_cast<std::vector<dal::level>*>(ctx)->push_back(level);

  return SQLITE_OK;
}

void
Offline::get_levels(std::function<void(struct levels)> cb)
{
  levels ret;
  if (SQLITE_OK != sqlite3_exec(db, "SELECT id, spec FROM levels", level_db_row, &ret.list, NULL)) {
    throw Exception("Could not fetch level specs from database.");
  }

  cb(ret);
}

void
Offline::get_completed_levels(std::function<void(struct completed_levels)> cb)
{
  completed_levels ret;

  const char *query = "SELECT levelid FROM completed_levels WHERE username = ?";

  sqlite3_stmt *stmt;
  if (SQLITE_OK != sqlite3_prepare_v2(db, query, -1, &stmt, NULL)) {
    throw Exception("Could not prepare statement.");
  }

  if (SQLITE_OK != sqlite3_bind_text(stmt, 1,
        ::User::instance().get_username().c_str(), -1, SQLITE_STATIC)) {
    throw Exception("Could not bind data to statement.");
  }

  while (SQLITE_ROW == sqlite3_step(stmt)) {
    level l;
    l.id = sqlite3_column_int(stmt, 0);
    ret.list.push_back(l);
  }

  cb(ret);
}

E_NS_DAL
