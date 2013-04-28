#include "SphereCreep.h"
#include "Model.h"

#include <iostream>
#include <OpenGL/gl.h>

Model *model = NULL;

SphereCreep::SphereCreep() : Creep(80, 10, 1, 1.3)
{
	if (NULL == model) model = new Model("models/spherecreep/spherecreep.dae");
}

void
SphereCreep::draw_model(const float& elapsed) const
{
	glScalef(0.25, 0.25, 0.25);
	model->draw(elapsed);
}
