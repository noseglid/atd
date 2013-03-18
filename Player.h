#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Listener.h"
#include "Purchasable.h"

class Player : public Listener
{
	int gold;

	Player();
	void operator=(Player const&);
	Player(const Player&);

public:
	static Player& instance();

	void alter_gold(int delta);

	bool purchase(Purchasable *p);
	void event(const float& elapsed);
};

#endif
