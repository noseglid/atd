#ifndef __CREEP_H__
#define __CREEP_H__

#include "Map.h"

class Creep
{
	void travel_to(const PathCoord& target);

protected:
	Map *map;
	const Path *path;
	PathCoord target;

	float x, y;
	float dx, dy;

	Creep(Map *map);

public:
	virtual void draw(const float& elapsed) const = 0;
	void tick(const float& elapsed);
};

#endif
