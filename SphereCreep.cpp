#include "SphereCreep.h"
#include "Model.h"

#include <iostream>
#include <OpenGL/gl.h>

Model *model = NULL;

SphereCreep::SphereCreep(Map *map, float health) : Creep(map, health, 20, 1)
{
	if (NULL == model) model = new Model("models/spherecreep/spherecreep.dae");
}

void
SphereCreep::draw_model(const float& elapsed) const
{
	glScalef(0.25, 0.25, 0.25);
	model->draw(elapsed);
}
