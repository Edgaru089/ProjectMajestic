#pragma once

#include "Entity.hpp"


class Mob :public Entity {
public:

	Mob() :Entity() {}
	virtual ~Mob() {}

	virtual const int getMaxHealth() = 0;
	virtual const double getShieldFactor() { return 1.0; }

	void harm(int amount) {
		// TODO Particles on harm
		harmUnprotected(amount*getShieldFactor());
	}

	void harmUnprotected(int amount) {
		if (health() <= amount) {
			health() = 0;
			kill();
		}
		else {
			health() -= amount;
		}
	}

	int getHealth() { return health(); }

protected:

	int& health() { return datasets["mob_health"].getDataInt(); };

};
