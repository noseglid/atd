#ifndef __PATH_H__
#define __PATH_H__

#include <vector>
#include <map>

struct PathCoord
{
	int x, y;

	PathCoord(int x, int y) : x(x), y(y) {}
	PathCoord() : PathCoord(0, 0) {}

	PathCoord& operator=(const PathCoord& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}

	bool operator==(const PathCoord& rhs) const
	{
		return rhs.x == x && rhs.y == y;
	}

	bool operator!=(const PathCoord& rhs) const
	{
		return !(*this == rhs);
	}

	bool operator<(const PathCoord& rhs) const
	{
		if (x != rhs.x) return x < rhs.x;
		return y < rhs.y;
	}
};

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
	bool has_coord(const PathCoord& coord);
	bool has_coord(int x, int y);
	PathCoord get_start() const;
};

#endif
