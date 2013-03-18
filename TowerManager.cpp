#include "TowerManager.h"
#include "Text.h"
#include "Player.h"
#include "Creep.h"
#include "Game.h"

TowerManager::TowerManager(Map *map) : map(map)
{
	Game::instance().register_listener(this)
		<< LE_TICK
		<< SDL_MOUSEBUTTONUP
		<< SDL_MOUSEBUTTONDOWN;

	Tower::init();
}

void
TowerManager::purchase_tower(Vector3 pos)
{
	Tower *t = new Tower(pos);
	if (Player::instance().purchase(t)) {
		towers.insert(std::make_pair(pos, t));
	} else {
		Text::scrolling("Not enough gold!", pos);
		delete t;
	}
}

void
TowerManager::event(const SDL_MouseButtonEvent& event)
{
	static int x, y;

	if (SDL_MOUSEBUTTONDOWN == event.type) {
		x = event.x;
		y = event.y;
	} else if (SDL_MOUSEBUTTONUP == event.type) {
		if (event.button == SDL_BUTTON_LEFT &&
				abs(event.x - x) < 3 &&
				abs(event.y - y) < 3) {
			/* Since coordinates didn't change, it was a `click`, not a `drag` */
			Vector2 hl = map->get_highlight();
			Vector3 pos(hl.x + 0.5f, 0.0f, hl.y + 0.5f);
			if (towers.end() == towers.find(pos) && hl.x != -1 && hl.y != -1) {
				purchase_tower(pos);
			}
		}
	}
}

void
TowerManager::event(const float& elapsed)
{
	for (std::pair<Vector3, Tower*> t : towers) {

		t.second->shoot_if(elapsed);
		t.second->update_projectiles(elapsed);

		glPushMatrix();
		t.second->draw(elapsed);
		glPopMatrix();
	}
}
