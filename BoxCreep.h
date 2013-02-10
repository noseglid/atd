#ifndef __BOXCREEP_H__
#define __BOXCREEP_H__

#include "Creep.h"
#include "Model.h"

class BoxCreep : public Creep
{
public:
	BoxCreep(Map *map);
	void draw(const float& elapsed) const;
};

#endif
