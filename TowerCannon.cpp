#include "TowerCannon.h"
#include "Debug.h"

TowerCannon::TowerCannon(Vector3 pos) : Tower(100, pos, 2.0f, 4.0f, 45.0f)
{
	this->model = Model::load("models/tower_cannon/tower_cannon.dae");
}
