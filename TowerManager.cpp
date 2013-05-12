#include "TowerManager.h"
#include "IO.h"
#include "ImageLoader.h"
#include "Text.h"
#include "Player.h"
#include "Creep.h"
#include "Game.h"
#include "HUD.h"
#include "Faction.h"

TowerManager::TowerManager()
{
	Game& g = Game::instance();
	g.on("tick",      std::bind(&TowerManager::tick,      this, std::placeholders::_1));
	g.on("mousedown", std::bind(&TowerManager::mousedown, this, std::placeholders::_1));
	g.on("mouseup",   std::bind(&TowerManager::mouseup,   this, std::placeholders::_1));
	g.on("keydown",   std::bind(&TowerManager::keydown,   this, std::placeholders::_1));

	Map& m = Map::instance();
	m.on("hover", std::bind(&TowerManager::map_hover, this, std::placeholders::_1));

	audio_build = Audio::instance().load_sfx("sfx/build1.ogg");
}

void
TowerManager::set_faction(Faction::Faction faction)
{
	std::list<std::string> specs = Faction::Loader::tower_specs(faction);
	for (std::string file : specs) {
		Json::Value jspec = Json::deserialize(IO::file_get_contents(file));
		available_towers.insert(std::make_pair(jspec["name"].asString(), jspec));
		HUD::instance().add_button(
			IL::GL::texture(jspec["hudtex"].asString()),
			std::bind(
				&TowerManager::select_tower,
				this,
				jspec["name"].asString(),
				std::placeholders::_1
			)
		);
		Tower *t = create_tower(jspec["name"].asString(), Vector3());
		dummy_towers.insert(std::make_pair(jspec["name"].asString(), t));
	}
}

Tower *
TowerManager::create_tower(std::string tower, Vector3 pos)
{
	Json::Value spec = available_towers.at(tower);
	return new Tower(spec, pos);
}

void
TowerManager::dummy_tower(int x, int y)
{
	Map& map = Map::instance();
	Vector3 pos = map.get_center_of(x, y);
	pos.y = 0.0f;
	int hlx = x, hly = y;

	if (map.get_path()->has_coord(x, y) ||
	    towers.end() != towers.find(pos) ||
	    x == 0 || x == map.get_width() - 1 ||
	    y == 0 || y == map.get_height() - 1) {
		/* This is not a valid coord for a dummy tower, reset all params */
		pos = Vector3(0.0f, 0.0f, 0.0f);
		hlx = -1;
		hly = -1;
	}

	dummy_tower_pos = pos;
	map.set_highlight(hlx, hly);
}

void
TowerManager::map_hover(const MapEvent& me)
{
	last_map_event = me;
	if (selected_tower.empty())
		return;

	dummy_tower(me.hovered.x, me.hovered.y);
}

void
TowerManager::deselect_tower()
{
	selected_tower = "";
	HUD::instance().mark_button(-1);
	Map::instance().set_highlight(-1, -1);
}

void
TowerManager::select_tower(std::string tower, int i)
{
	selected_tower = tower;
	HUD::instance().mark_button(i);
	Map::instance().set_highlight(-1, -1);
	dummy_tower(last_map_event.hovered.x, last_map_event.hovered.y);
}

bool
TowerManager::purchase_tower(Vector3 pos)
{
	if (towers.end() != towers.find(pos)) {
		return false;
	}

	Tower *t = create_tower(selected_tower, pos);
	if (!Player::instance().purchase(t)) {
		Text::scrolling("You're broke mate!", pos);
		delete t;
		return false;
	}

	towers.insert(std::make_pair(pos, t));
	dummy_tower_pos = Vector3();
	Map::instance().set_highlight(-1, -1);

	return true;
}

void
TowerManager::keydown(const GameEvent& ge)
{
	switch (ge.ev.key.keysym.sym) {
	case SDLK_ESCAPE:
		deselect_tower();
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
		if (0.0f >= hl.x || 0.0f >= hl.y) return;

		Vector3 pos = Map::instance().get_center_of(hl.x, hl.y);
		pos.y = 0.0f;
		bool purchased = purchase_tower(pos);
		if (purchased) {
			Audio::instance().play(audio_build, 3);
		}

		SDLMod mod = SDL_GetModState();
		if (!(mod & KMOD_SHIFT) && purchased) {
			deselect_tower();
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

	if (!selected_tower.empty() && dummy_tower_pos.length() > 0.0f) {
		Tower *t = dummy_towers.at(selected_tower);
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
