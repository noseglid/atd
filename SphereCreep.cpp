#include "SphereCreep.h"
#include "Model.h"
#include "Audio.h"

#include <iostream>
#include <OpenGL/gl.h>

Model *model = Model::load("spherecreep/spherecreep.dae");

SphereCreep::SphereCreep() :
  Creep(80, 10, 1, 1.3, Audio::instance().load_sfx("sfx/creep_death.ogg"))
{
}

void
SphereCreep::draw_model(const float& elapsed) const
{
  glScalef(0.25, 0.25, 0.25);
  model->draw(elapsed);
}
