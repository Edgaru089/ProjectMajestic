#pragma once

#include "GrenadeEntity.hpp"
#include "EntityManager.hpp"
#include "PlayerEntity.hpp"


////////////////////////////////////////
void GrenadeEntity::_onCollideEntity(Entity* e) {
	if (!isAlive())
		return;
	if (dynamic_cast<Mob*>(e) != nullptr)
		if (e->getUuid() != localPlayer->getUuid()) {
			entityManager.explode(getCenterPos(), grenadeDamage);
			kill();
		}
}


////////////////////////////////////////
void GrenadeEntity::_onCollision(Block* b) {
	if (!isAlive())
		return;
	if (b->isSolid()) {
		entityManager.explode(getCenterPos(), grenadeDamage);
		kill();
	}
}

