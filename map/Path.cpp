#include "map/Path.h"
#include "map/NoMoreCoords.h"
#include "Debug.h"
#include "Exception.h"

#include <sstream>

namespace map {

static auto clamp1 = [](int n)
{
  if (n >= 1) return 1;
  if (n <= -1) return -1;
  return 0;
};

void
Path::path_line(PathCoord& current, const PathCoord& target)
{
  if (current.x != target.x && current.y != target.y) {
    throw Exception("Can not move in both x and y led simultaneously.");
  }

  int dx = clamp1(target.x - current.x);
  int dy = clamp1(target.y - current.y);

  do {
    current.x += dx;
    current.y += dy;
    coords_order.push_back(current);
  } while (current != target);
}

Path::Path(int textureid, std::string path) : textureid(textureid)
{
  std::stringstream ss(path);
  ss.exceptions();

  PathCoord cur;
  while (ss.good()) {
    char op, dummy;
    PathCoord coord;
    ss >> op >> coord.x >> dummy >> coord.y;
    switch (op) {
      case 'M':
        cur = coord;
        coords_order.push_back(cur);
        break;
      case 'L':
        path_line(cur, coord);
        break;
      default:
        std::string exc("Unsupported operator: ");
        exc += op;
        throw Exception(exc);
    }
  }

  for (PathCoord c : coords_order) {
    active_coords[c] = true;
  }
}

PathCoord
Path::next_coord(const PathCoord& coord) const
{
  auto it = find(coords_order.begin(), coords_order.end(), coord);

  if (it == coords_order.end())
    throw Exception("Specified coord is not in map.");

  if ((it + 1) == coords_order.end())
    throw NoMoreCoords();

  return *(it + 1);
}

bool
Path::has_coord(const PathCoord& coord) const
{
  return active_coords.find(coord) != active_coords.end();
}

bool
Path::has_coord(int x, int y) const
{
  PathCoord coord(x, y);
  return has_coord(coord);
}

PathCoord
Path::get_start() const
{
  return coords_order[0];
}

}
