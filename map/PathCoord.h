#ifndef __MAP__PATHCOORD_H__
#define __MAP__PATHCOORD_H__

#include <ostream>

namespace map {

struct PathCoord
{
  int x, y;

  PathCoord(int x, int y);
  PathCoord();

  PathCoord& operator=(const PathCoord& rhs);
  bool operator==(const PathCoord& rhs) const;
  bool operator!=(const PathCoord& rhs) const;
  bool operator<(const PathCoord& rhs) const;
};

std::ostream& operator<<(std::ostream& out, const PathCoord& v);

}

#endif
