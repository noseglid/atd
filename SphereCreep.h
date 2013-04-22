#ifndef __BOXCREEP_H__
#define __BOXCREEP_H__

#include "Creep.h"

class SphereCreep : public Creep
{
public:
	SphereCreep();
	void draw_model(const float& elapsed) const;
};

#endif
