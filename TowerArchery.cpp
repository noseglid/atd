#include "TowerArchery.h"
#include "Debug.h"

static Model *smodel = NULL;

TowerArchery::TowerArchery(Vector3 pos) : Tower(75, pos, 0.7f, 2.0f, 15.7)
{
	if (NULL == smodel) {
		DBGERR("Model have not been initalized.");
		throw Exception("Model have not been initialized.");
	}

	this->model = smodel;
}

void
TowerArchery::init()
{
	smodel = new Model("models/tower_archery/tower_archery.dae");
}
