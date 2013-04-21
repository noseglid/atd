#include "TowerManager.h"
#include "ImageLoader.h"
#include "Text.h"
#include "Player.h"
#include "Creep.h"
#include "Game.h"
#include "HUD.h"

/* Available towers */
#include "TowerArchery.h"
#include "TowerCannon.h"

TowerManager::TowerManager(Map *map) : current_tower(TOWER_ARCHERY), map(map)
{
	Game& g = Game::instance();
	g.on("tick", std::bind(&TowerManager::tick, this, std::placeholders::_1));
	g.on("mousedown", std::bind(&TowerManager::mousedown, this, std::placeholders::_1));
	g.on("mouseup", std::bind(&TowerManager::mouseup, this, std::placeholders::_1));

	TowerArchery::init();
	TowerCannon::init();

	HUD::instance().add_button(IL::GL::texture("tower_hero.jpg"),
		std::bind(&TowerManager::select_tower, this, TOWER_ARCHERY, std::placeholders::_1));
	HUD::instance().add_button(IL::GL::texture("tower_basic1.jpg"),
		std::bind(&TowerManager::select_tower, this, TOWER_CANNON, std::placeholders::_1));
}

void
TowerManager::select_tower(TOWER_TYPE t, int i)
{
	current_tower = t;
}

void
TowerManager::purchase_tower(Vector3 pos)
{
	Tower *t;
	switch (current_tower) {
	case TOWER_ARCHERY: t = new TowerArchery(pos); break;
	case TOWER_CANNON:  t = new TowerCannon(pos); break;
	}

	if (!Player::instance().purchase(t)) {
		Text::scrolling("You're broke mate!", pos);
		delete t;
		return;
	}

	towers.insert(std::make_pair(pos, t));
}

void
TowerManager::mousedown(const GameEvent& ev)
{
	SDL_MouseButtonEvent event = ev.ev.button;
	click.x = event.x;
	click.y = event.y;
}

void
TowerManager::mouseup(const GameEvent& ge)
{
	SDL_MouseButtonEvent event = ge.ev.button;

	if (HUD::instance().in_turf(event.x, event.y)) return;

	if (event.button == SDL_BUTTON_LEFT &&
			abs(event.x - click.x) < 3 &&
			abs(event.y - click.y) < 3) {
		/* Since coordinates didn't change (granted, some fuzz), it was a `click`, not a `drag` */
		Vector2 hl = map->get_highlight();
		Vector3 pos(hl.x + 0.5f, 0.0f, hl.y + 0.5f);
		if (towers.end() == towers.find(pos) && hl.x != -1 && hl.y != -1) {
			purchase_tower(pos);
		}
	}
}

void
TowerManager::tick(const GameEvent& ev)
{
	float elapsed = ev.elapsed;

	for (std::pair<Vector3, Tower*> t : towers) {

		t.second->shoot_if(elapsed);
		t.second->update_projectiles(elapsed);

		glPushMatrix();
		t.second->draw(elapsed);
		glPopMatrix();
	}
}
