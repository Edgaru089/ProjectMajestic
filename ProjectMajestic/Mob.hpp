#pragma once

#include "Entity.hpp"


class Mob :public Entity {
public:

	virtual const int getMaxHealth() = 0;

	void harm(int amount) {
		// TODO Particles on harm
		if (health <= amount) {
			health = 0;
			kill();
		}
		else {
			health -= amount;
		}
	}

protected:

	int health;

};
