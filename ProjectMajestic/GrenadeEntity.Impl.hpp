#pragma once

#include "GrenadeEntity.hpp"
#include "EntityManager.hpp"
#include "PlayerEntity.hpp"


////////////////////////////////////////
void GrenadeEntity::_onCollideEntity(shared_ptr<Entity> e) {
	if (!isAlive())
		return;
	try {
		Mob& mob = dynamic_cast<Mob&>(*e);
		if (mob.getUuid() != localPlayer->getUuid()) {
			entityManager.explode(getCenterPos(), grenadeDamage);
			kill();
		}
	}
	catch (bad_cast) {}
}


////////////////////////////////////////
void GrenadeEntity::_onCollision(shared_ptr<Block> b) {
	if (!isAlive())
		return;
	if (b->isSolid()) {
		entityManager.explode(getCenterPos(), grenadeDamage);
		kill();
	}
}

