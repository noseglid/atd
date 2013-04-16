#include "TowerCannon.h"
#include "Debug.h"

static Model *smodel = NULL;

TowerCannon::TowerCannon(Vector3 pos) : Tower(100, pos, 2.0f, 4.0f, 45.0f)
{
	if (NULL == smodel) {
		DBGERR("Model have not been initalized.");
		throw Exception("Model have not been initialized.");
	}

	this->model = smodel;
}

void
TowerCannon::init()
{
	smodel = new Model("models/tower_cannon/tower_cannon.dae");
}
