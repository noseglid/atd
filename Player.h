#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Purchasable.h"

class Player
{
	int gold;

	Player();
	void operator=(Player const&);
	Player(const Player&);

public:
	static Player& instance();

	void alter_gold(int delta);

	bool purchase(Purchasable *p);
	void tick();
};

#endif
