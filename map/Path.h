#ifndef __MAP__PATH_H__
#define __MAP__PATH_H__

#include "map/PathCoord.h"
#include "Exception.h"

#include <vector>
#include <map>
#include <iostream>

namespace map {

class Path
{
public:
  int textureid;

private:
  std::vector<PathCoord> coords_order;
  std::map<PathCoord, bool> active_coords;

  void path_line(PathCoord& current, const PathCoord& target);
public:
  Path(int textureid, std::string path);

  PathCoord next_coord(const PathCoord& coord) const;
  bool has_coord(const PathCoord& coord) const;
  bool has_coord(int x, int y) const;
  PathCoord get_start() const;
};

}

#endif
