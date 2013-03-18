#include "Player.h"
#include "Game.h"
#include "Text.h"

#include <sstream>

Player::Player() : gold(0)
{
	Game::instance().register_listener(this)
		<< LE_TICK;
}

Player&
Player::instance()
{
	static Player instance;
	return instance;
}

void
Player::alter_gold(int delta)
{
	this->gold += delta;
}

bool
Player::purchase(Purchasable *p)
{
	if (p->price > this->gold)
		return false;

	alter_gold(-p->price);
	return true;
}

void
Player::event(const float& elapsed)
{
	std::stringstream overlay;
	overlay << "Gold: " << this->gold;
	Text::overlay(overlay.str(), 10, 30, false);
}
