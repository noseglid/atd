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

TowerManager::TowerManager() : current_tower(TOWER_NONE)
{
	Game& g = Game::instance();
	g.on("tick", std::bind(&TowerManager::tick, this, std::placeholders::_1));
	g.on("mousedown", std::bind(&TowerManager::mousedown, this, std::placeholders::_1));
	g.on("mouseup", std::bind(&TowerManager::mouseup, this, std::placeholders::_1));
	g.on("keydown", std::bind(&TowerManager::keydown, this, std::placeholders::_1));

	Map& m = Map::instance();
	m.on("hover", std::bind(&TowerManager::map_select, this, std::placeholders::_1));

	TowerArchery::init();
	TowerCannon::init();

	for (TOWER_TYPE t = TOWER_FIRST; ++t != TOWER_NONE;) {
		dummy_towers.insert(std::make_pair(t, create_tower(t, Vector3())));
	}

	HUD& h = HUD::instance();
	h.add_button(IL::GL::texture("tower_hero.jpg"),
		std::bind(&TowerManager::select_tower, this, TOWER_ARCHERY, std::placeholders::_1));
	h.add_button(IL::GL::texture("tower_basic1.jpg"),
		std::bind(&TowerManager::select_tower, this, TOWER_CANNON, std::placeholders::_1));
}

Tower *
TowerManager::create_tower(TOWER_TYPE type, Vector3 pos)
{
	Tower *tower = NULL;
	switch (type) {
	case TOWER_ARCHERY: tower = new TowerArchery(pos); break;
	case TOWER_CANNON:  tower = new TowerCannon(pos); break;
	default: break;
	}
	return tower;
}

void
TowerManager::map_select(const MapEvent& me)
{
	if (TOWER_NONE == current_tower)
		return;

	Map& map = Map::instance();
	int x = me.hovered.x, y = me.hovered.y;
	Vector3 pos = map.get_center_of(x, y);
	pos.y = 0.0f;
	int hlx = x, hly = y;

	if (map.get_path()->has_coord(x, y) || towers.end() != towers.find(pos)) {
		/* This is not a valid coord for anything, reset all params */
		pos = Vector3(0.0f, 0.0f, 0.0f);
		hlx = -1;
		hly = -1;
	}

	dummy_tower_pos = pos;
	map.set_highlight(hlx, hly);
}

void
TowerManager::select_tower(TOWER_TYPE t, int i)
{
	current_tower = t;
	HUD::instance().mark_button(i);
	Map::instance().set_highlight(-1, -1);

	dummy_tower_pos = Vector3();
}

void
TowerManager::purchase_tower(Vector3 pos)
{
	if (towers.end() != towers.find(pos)) {
		return;
	}

	Tower *t = create_tower(current_tower, pos);
	if (!Player::instance().purchase(t)) {
		Text::scrolling("You're broke mate!", pos);
		delete t;
		return;
	}

	towers.insert(std::make_pair(pos, t));
	dummy_tower_pos = Vector3();
	Map::instance().set_highlight(-1, -1);
}

void
TowerManager::keydown(const GameEvent& ge)
{
	switch (ge.ev.key.keysym.sym) {
	case SDLK_ESCAPE:
		select_tower(TOWER_NONE, -1);
		break;
	default:
		break;
	}
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

		/* Since coordinates didn't change (granted, some fuzz),
		 * it was a `click`, not a `drag` */

		Vector2 hl = Map::instance().get_highlight();
		if (0.0f >= hl.x || 0.0f >= hl.y)
			return;

		Vector3 pos = Map::instance().get_center_of(hl.x, hl.y);
		pos.y = 0.0f;
		purchase_tower(pos);

		SDLMod mod = SDL_GetModState();
		if (!(mod & KMOD_SHIFT)) {
			select_tower(TOWER_NONE, -1);
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

	if (current_tower != TOWER_NONE && dummy_tower_pos.length() > 0.0f) {
		Tower *t = dummy_towers[current_tower];
		t->set_position(dummy_tower_pos);

		glEnable(GL_COLOR_MATERIAL);
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);

		glPushMatrix();
			t->draw(elapsed);
		glPopMatrix();

		glDisable(GL_COLOR_MATERIAL);
	}
}

TowerManager&
TowerManager::instance()
{
	static TowerManager instance;
	return instance;
}
