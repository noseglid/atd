#include "Creep.h"

Creep::Creep(Map *map)
{
	this->map  = map;
	this->path = map->get_path();

	PathCoord start = path->get_start();
	Vector3 start_center = map->get_center_of(start.x, start.y);

	x = start_center.x;
	y = start_center.z;

	travel_to(path->next_coord(start));
}

void
Creep::travel_to(const PathCoord& target)
{
	this->target = target;

	Vector3 target_center = map->get_center_of(target.x, target.y);

	float xdiff = (float)(target_center.x - x);
	float ydiff = (float)(target_center.z - y);
	dx =  xdiff / 20.0f;
	dy =  ydiff / 20.0f;
}

void
Creep::tick(const float& elapsed)
{
	Vector3 tc = map->get_center_of(target.x, target.y);
	if ((dx > 0 && x > tc.x) || (dx < 0 && x < tc.x) ||
	    (dy > 0 && y > tc.z) || (dy < 0 && y < tc.z)) {
		travel_to(path->next_coord(target));
	}

	x += dx;
	y += dy;
}
