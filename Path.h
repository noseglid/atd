#ifndef __PATH_H__
#define __PATH_H__

#include "Exception.h"

#include <vector>
#include <map>
#include <iostream>

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

std::ostream& operator<<(std::ostream& out, const PathCoord& v);

class NoMoreCoords : public Exception
{
public:
	NoMoreCoords() : Exception("No more coordinates in path.") {}
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
	bool has_coord(const PathCoord& coord) const;
	bool has_coord(int x, int y) const;
	PathCoord get_start() const;
};

#endif
