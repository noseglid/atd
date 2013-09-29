#include "dal/Offline.h"
#include "Exception.h"
#include "Hash.h"
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
level_db_row(void *cl, int ncols, char **values, char **colnames)
{
  if (1 != ncols) {
    DBGERR("Got more than one row in query");
    return SQLITE_ABORT;
  }

  std::vector<Json::Value> *ctx = static_cast<std::vector<Json::Value>*>(cl);
  ctx->push_back(Json::deserialize(values[0]));

  return SQLITE_OK;
}

void
Offline::get_levels(std::function<void(struct levels)> cb)
{
  levels ret;
  if (SQLITE_OK != sqlite3_exec(db, "SELECT spec FROM levels", level_db_row, &ret.specs, NULL)) {
    throw Exception("Could not fetch level specs from database.");
  }

  cb(ret);
}

void
Offline::get_completed_levels(std::string user, std::function<void(struct completed_levels)> cb)
{
  DBG("Getting offline completed levels");
}

}

E_NS_DAL
