#ifndef __TOWERCANNON_H__
#define __TOWERCANNON_H__

#include "Tower.h"

class TowerCannon : public Tower
{
public:
	TowerCannon(Vector3 pos);
	static void init();
};

#endif
