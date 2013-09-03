#include "dal/Offline.h"
#include "Exception.h"
#include "IO.h"
#include "Debug.h"

namespace dal {

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
Offline::get_user(std::string suser, std::function<void(struct user)> cb)
{
  DBG("Getting offline user");
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
