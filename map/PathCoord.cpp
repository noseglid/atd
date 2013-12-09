#include "map/PathCoord.h"

namespace map {

PathCoord::PathCoord(int x, int y) : x(x), y(y)
{
}

PathCoord::PathCoord() : PathCoord(0, 0)
{
}

PathCoord&
PathCoord::operator=(const PathCoord& rhs)
{
  x = rhs.x;
  y = rhs.y;
  return *this;
}

bool
PathCoord::operator==(const PathCoord& rhs) const
{
  return (rhs.x == x && rhs.y == y);
}

bool
PathCoord::operator!=(const PathCoord& rhs) const
{
  return !(*this == rhs);
}

bool
PathCoord::operator<(const PathCoord& rhs) const
{
  if (x != rhs.x) return x < rhs.x;
  return y < rhs.y;
}

std::ostream&
operator<<(std::ostream& out, const PathCoord& v)
{
  return out << v.x << "," << v.y;
}

}
