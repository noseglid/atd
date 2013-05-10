#include "TowerArchery.h"
#include "Debug.h"

TowerArchery::TowerArchery(Vector3 pos) : Tower(75, pos, 0.7f, 2.0f, 15.7)
{
	this->model = Model::load("models/tower_archery/tower_archery.dae");
}
