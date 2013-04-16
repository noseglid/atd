#ifndef __TOWERARCHERY_H__
#define __TOWERARCHERY_H__

#include "Tower.h"

class TowerArchery : public Tower
{
public:
	TowerArchery(Vector3 pos);
	static void init();
};

#endif
