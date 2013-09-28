#!/bin/sh

set -e

DB=$1
SQLCMD="sqlite3 -echo $DB"

if [ -z $DB ]; then
  echo "Usage: $0 <database>"
  exit 1
fi

$SQLCMD "DROP TABLE IF EXISTS users"
$SQLCMD "$(cat << 'EOF'
CREATE TABLE IF NOT EXISTS users (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  username TEXT,
  password TEXT
)
EOF)"
$SQLCMD "INSERT INTO users(username, password) VALUES ('u', '516b9783fca517eecbd1d064da2d165310b19759')" #SHA1 of 'p'

$SQLCMD "DROP TABLE IF EXISTS levels"
$SQLCMD "$(cat << 'EOF'
CREATE TABLE levels (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  spec TEXT
)
EOF)"

$SQLCMD "DROP TABLE IF EXISTS levels_completed"
$SQLCMD "$(cat << 'EOF'
CREATE TABLE levels_completed (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  userid INTEGER,
  levelis INTEGER
)
EOF)"

# Set database to old modify time so it's updated
touch -m -t 199001010001 $DB
make -C resources/levels/
